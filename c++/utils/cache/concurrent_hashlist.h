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
    }
    void Reset(T *t) { value.reset(t); }

    std::unique_ptr<T> value;
    ConcurrentHashListNode<T, Mutex> *pred {nullptr};
    ConcurrentHashListNode<T, Mutex> *next {nullptr};
    volatile NodeMode mode {MODE_NOT_AVAILABLE};
    Mutex mtx;
    size_t timeout {NEVER_TIMEOUT}; 
};

#define LockWithTryLoop(node, ori) do { \
    bool locked{false};                 \
    node = ori;                         \
    locked = node->mtx.try_lock();      \
    if (locked ||                       \
        node->mode == MODE_AVAILABLE) { \
        break;                          \
    }                                   \
    if (locked) { node->mtx.unlock(); } \
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

    ~ConcurrentHashList() {
        Node *cur{nullptr};
        while((cur = PopHead()) { delete cur; }
    }

    bool PushBack(Node *node) {
        std::lock_guard<Mutex> l1(end_.mtx);
        Node *ep{nullptr};
        LockWithTryLoop(ep, end_.pred);
        node->mode = MODE_AVAILABLE;
        ep->next = node;
        node->pred = end_.pred;
        node->next = &end_;
        ep = node;
        ep->mtx.unlock();
        return true;
    }

    Node *PopHead() {
        Node *hn{nullptr};
        LockWithTryLoop(hn, head_.next);
        head_.mtx.lock();
        Node *ret{nullptr};
        if (head_.next != &end_) {
            ret = head_.next;
            head_.next = ret->next;
            ret->next->pred = head_;
            ret->mode = MODE_NOT_AVAILABLE;
        }
        head_.mtx.unlock();
        hn->mtx.unlock();
        return ret;
    }

    Node *DetachNode(Node *node) {
        Node *n{nullptr}, *p{nullptr};
        LockWithTryLoop(n, node->next);
        node->mtx.lock();
        LockWithTryLoop(p, node->pred);
        p->next = n;
        n->pred = p;
        p->mtx.unlock();
        node->mtx.unlock();
        n->mtx.unlock();
        return node;
    }

    Node begin_;
    Node end_;
};

template<typename T,
         typename Mutex = SpinMutex>
class NodeManager {
 public:
    using List = ConcurrentHashList<T, Mutex>;
    using Node = List::Node;
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
        running = false;
        reclaim_thread_.join();
    }

    Node *GetOne() {
        Node *ret = resouce_.PopHead();
        if (!ret) {
            ret = new Node();
            ret->mode = MODE_AVAILABLE;
            resouce_.PushBack(ret);
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
        Node *cur {&reclaimer_.head_}, *last{nullptr};
        do {
            last = cur;
            cur = cur->next;
            if (cur == &reclaimer_.end_ ||
                cur ->timeout > expire_time) { break; }
            cur->mode = MODE_AVAILABLE;
        } while (true);
        if (last == &reclaimer_.head_) { return false; }
        cur = reclaimer_.head_.next;
        do {
            Node *tmp{nullptr};
            LockWithTryLoop(tmp, last->next);
            reclaimer_.head_.next = tmp;
            tmp->pred = &reclaimer_.head_;
            tmp->mtx.unlock();
        } while(0);

        do {
            std::lock_guard<Mutex> _(resouce_.end_.mtx);
            Node *tmp{nullptr};
            LockWithTryLoop(tmp, resouce_.end_.pred);
            tmp->next = cur;
            cur->pred = tmp;
            last->next = &resouce_.end_;
            resouce_.end_.pred = last;
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
