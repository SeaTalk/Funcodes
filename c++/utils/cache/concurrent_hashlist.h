#pragma once
#include <ctime>
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include "spinlock.h"

enum NodeMode {
    MODE_AVAILABLE,
    MODE_NOT_AVAILABLE
};

#define NEVER_TIMEOUT 0x8000000000000000

template<typename T,
         typename Mutex = SpinMutex>
struct ConcurrentHashListNode {
    ConcurrentHashListNode() { }
    template<typename ...Args>
    ConcurrentHashListNode(Args&&... args) :
            value(new T(std::forward<Args>(args)...)) { }

    template<typename ...Args>
    void Reset(Args&&... args) {
        value.reset(new T(std::forward<Args>(args)...));
        // std::cout << "reset value:" << value.get() << std::endl;
    }
    void Reset(T *t) { value.reset(t); }

    std::unique_ptr<T> value;
    std::atomic<ConcurrentHashListNode<T, Mutex> *> pred {nullptr};
    std::atomic<ConcurrentHashListNode<T, Mutex> *> next {nullptr};
    volatile NodeMode mode {MODE_NOT_AVAILABLE};
    Mutex mtx;
    size_t timeout {NEVER_TIMEOUT}; 
};

#define LockWithTryLoop(node, ori) do { \
    bool locked{false};                 \
    node = ori.load(std::memory_order_acquire);\
    locked = node->mtx.try_lock();      \
    if (locked) { break; }              \
} while (true)

template<typename T,
         typename Mutex = SpinMutex>
struct ConcurrentHashList {
    using Node = ConcurrentHashListNode<T, Mutex>;
    ConcurrentHashList() : begin_(), end_() {
        begin_.mode = MODE_AVAILABLE;
        end_.mode = MODE_AVAILABLE;
        begin_.next = &end_;
        end_.pred = &begin_;
    }

    bool PushBack(Node *node) {
        std::lock_guard<Mutex> l1(end_.mtx);
        Node *ep{nullptr};
        LockWithTryLoop(ep, end_.pred);
        node->mode = MODE_AVAILABLE;
        ep->next.store(node, std::memory_order_release);
        node->pred.store(ep, std::memory_order_release);
        node->next.store(&end_, std::memory_order_release);
        end_.pred.store(node, std::memory_order_release);
        ep->mtx.unlock();
        return true;
    }

    Node *PopHead() {
        Node *hn{nullptr};
        LockWithTryLoop(hn, begin_.next);
        begin_.mtx.lock();
        Node *ret = begin_.next.load(std::memory_order_acquire);
        if (ret != &end_) {
            Node *rn = ret->next.load(std::memory_order_acquire);
            begin_.next.store(rn, std::memory_order_release);
            rn->pred.store(&begin_, std::memory_order_release);
            ret->mode = MODE_NOT_AVAILABLE;
        }
        begin_.mtx.unlock();
        hn->mtx.unlock();
        return ret;
    }

    bool DetachNode(Node *node) {
        Node *n{nullptr}, *p{nullptr};
        bool ret {false};
        LockWithTryLoop(n, node->next);
        if (node->mode == MODE_AVAILABLE) {
            node->mtx.lock();
            node->mode = MODE_NOT_AVAILABLE;
            LockWithTryLoop(p, node->pred);
            p->next.store(n, std::memory_order_release);
            n->pred.store(p, std::memory_order_release);
            p->mtx.unlock();
            node->mtx.unlock();
            ret = true;
        }
        n->mtx.unlock();
        return ret;
    }

    Node begin_;
    Node end_;
};

template<typename T,
         typename Mutex = SpinMutex>
class NodeManager {
 public:
    using List = ConcurrentHashList<T, Mutex>;
    using Node = typename List::Node;
    NodeManager(size_t capacity, size_t window_s = 60, size_t slide_s = 1)
            : window_in_seconds_(window_s), slide_in_seconds_(slide_s),
              reclaim_thread_(&NodeManager<T, Mutex>::ReuseLoop, this) {
        for (size_t i = 0; i < capacity; ++i) {
            auto *node = new Node();
            node->mode = MODE_AVAILABLE;
            resouce_.PushBack(node);
        }
    }

    ~NodeManager() {
        running_ = false;
        reclaim_thread_.join();
    }

    Node *GetOne() {
        Node *ret = resouce_.PopHead();
        if (!ret) {
            ret = new Node();
            ret->mode = MODE_AVAILABLE;
            resouce_.PushBack(ret);
        } else {
            ret->next.store(nullptr, std::memory_order_release);
        }
        return ret;
    }

    bool Reclaim(Node *node) {
        node->mode = MODE_NOT_AVAILABLE;
        node->timeout = time(0) + window_in_seconds_;
        return running_ && reclaimer_.PushBack(node);
    }

    bool Reuse() {
        auto expired_time = time(0);
        Node *cur {&reclaimer_.begin_}, *last{nullptr};
        do {
            last = cur;
            cur = cur->next.load(std::memory_order_acquire);
            if (cur == &reclaimer_.end_ ||
                cur->timeout > expired_time) { break; }
            cur->mode = MODE_AVAILABLE;
        } while (true);
        if (last == &reclaimer_.begin_) { return false; }
        cur = reclaimer_.begin_.next.load(std::memory_order_acquire);
        do {
            Node *tmp{nullptr};
            LockWithTryLoop(tmp, last->next);
            reclaimer_.begin_.next.store(tmp, std::memory_order_release);
            tmp->pred.store(&reclaimer_.begin_, std::memory_order_release);
            tmp->mtx.unlock();
        } while(0);

        do {
            std::lock_guard<Mutex> _(resouce_.end_.mtx);
            Node *tmp{nullptr};
            LockWithTryLoop(tmp, resouce_.end_.pred);
            tmp->next.store(cur, std::memory_order_acquire);
            cur->pred.store(tmp, std::memory_order_release);
            last->next.store(&resouce_.end_, std::memory_order_release);
            resouce_.end_.pred.store(last, std::memory_order_release);
            tmp->mtx.unlock();
        } while(0);

        return true;
    }

    void ReuseLoop() {
        while(running_) {
            Reuse();
            std::this_thread::sleep_for(
                std::chrono::seconds(slide_in_seconds_));
        }
    }

 private:
    List resouce_;
    List reclaimer_;
    size_t window_in_seconds_;
    size_t slide_in_seconds_;
    volatile bool running_ {true};
    std::thread reclaim_thread_;
};
