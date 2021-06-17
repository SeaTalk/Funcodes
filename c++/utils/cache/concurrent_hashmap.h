#pragma once
#include <cmath>
#include <algorithm>
#include <memory>
#include <mutex>
#include <iostream>
#include <thread>
#include <vector>
#include <utility>
#include "rwlock.h"
#include "spinlock.h"

#define NEVER_TIMEOUT 0x8000000000000000

template<typename T, typename Mutex = SpinMutex>
struct ConcurrentHashListNode {
    ConcurrentHashListNode() { }
    template<typename ...Args>
    ConcurrentHashListNode(Args&&... args) : value(new T(std::forward<Args>(args)...)) { }

    template<typename ...Args>
    void Reset(Args&&... args) {
        value.reset(new T(std::forward<Args>(args)...));
    }
    void Reset(T *t) { value.reset(t); }

    std::unique_ptr<T> value;
    ConcurrentHashListNode<T, Mutex> *pred {nullptr};
    ConcurrentHashListNode<T, Mutex> *next {nullptr};
    size_t timeout {NEVER_TIMEOUT};
};

template<typename T, typename Mutex = SpinMutex>
struct ConcurrentHashList {
    typedef ConcurrentHashListNode<T> Node;
    ConcurrentHashList() : contention(new LockFreeRWContention()), begin(), end(&begin) { }

    ~ConcurrentHashList() {
        Node *cur{nullptr};
        while((cur = PopHead())) { delete cur; }
    }

    bool PushBack(Node *node) {
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(contention->GetWriteLock());
        end->next = node;
        node->pred = end;
        end = node;
        return true;
    }

    Node *PopHead() {
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(contention->GetWriteLock());
        if (end == &begin) { return nullptr; }
        auto *cur = begin.next;
        if (cur == end) {
            begin.next = nullptr;
            end = &begin;
        } else {
            begin.next = cur->next;
            cur->next->pred = &begin;
        }
        return cur;
    }

    bool DetachNode(Node *node) {
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(contention->GetWriteLock());
        if (node == end) {
            end = end->pred;
        } else {
            node->pred->next = node->next;
            node->next->pred = node->pred;
        }
        return true;
    }

    std::unique_ptr<LockFreeRWContention> contention;
    ConcurrentHashListNode<T, Mutex> begin;
    ConcurrentHashListNode<T, Mutex> *end;
};

template<typename T, typename Mutex = SpinMutex>
class TNodeManager {
public:
    using List = ConcurrentHashList<T>;
    using Node = typename List::Node;
    TNodeManager(size_t capacity, size_t window_s = 60, size_t slide_s = 1)
            : window_in_seconds_(window_s), slide_in_seconds_(slide_s),
            reclaim_thread_(&TNodeManager<T>::ReuseLoop, this) {
        for (size_t i = 0; i < capacity; ++i) {
            auto *node = new Node();
            resouce_.PushBack(node);
        }
    }

    ~TNodeManager() {
        running_ = false;
        reclaim_thread_.join();
    }

    Node *GetOne() {
        Node *ret = resouce_.PopHead();
        if (!ret) {
            ret = new Node();
            resouce_.PushBack(ret);
        } else {
            ret->next = nullptr;
        }
        return ret;
    }

    bool Reclaim(Node *node) {
        node->timeout = time(0) + window_in_seconds_;
        return running_ && reclaimer_.PushBack(node);
    }

    bool Reuse() {
        auto expired_time = time(0);
        Node *cur {&reclaimer_.begin}, *last{nullptr};
        do {
            last = cur;
            if (cur == reclaimer_.end ||
                cur ->timeout > expired_time) { break; }
            cur = cur->next;
        } while (true);
        if (last == &reclaimer_.begin) {
            return false; }
        cur = reclaimer_.begin.next;
        do {
            std::lock_guard<LockFreeRWContention::WriteLock>
                _(reclaimer_.contention->GetWriteLock());
            reclaimer_.begin.next = last->next;
            last->next->pred = &reclaimer_.begin;
        } while(0);
        do {
            std::lock_guard<LockFreeRWContention::WriteLock>
                _(resouce_.contention->GetWriteLock());
            Node *tmp = resouce_.end->pred;
            tmp->next = cur;
            cur->pred = tmp;
            last->next = nullptr;
            resouce_.end = last;
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

template<class Key, class Value, class Mutex = SpinMutex,
         class Hasher = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class ConcurrentHashMap {
 public:
    typedef std::pair<Key, Value> value_type;
    //  typedef value_type* iterator;
    // typedef const value_type* const_iterator;
    typedef ConcurrentHashList<value_type, Mutex> Bucket;
    typedef ConcurrentHashListNode<value_type, Mutex> Node;
    typedef TNodeManager<value_type, Mutex> NodeManager;

    class iterator {
     public:
        iterator(Node *node) : inner_(node) { }
        value_type &operator*() { return *inner_->value; }
        value_type *operator->() { return inner_->value.get(); }
        bool operator==(const iterator &it) {
            if (inner_ == it.inner_) { return true; }
            if (inner_ == nullptr || it.inner_ == nullptr) { return false; }
            return Equal()(inner_->value->first, it.inner_->value->first);
        }
        Node *Inner() { return inner_; }

     private:
        Node* inner_;
    };

    ConcurrentHashMap(size_t bucket_hint=3) :
            buckets_(FindAPrime(bucket_hint)), end_(nullptr),
            node_manager_(bucket_hint << 1) { }

    iterator find(const Key &key) const {
        size_t idx = HashResult(key);
        std::lock_guard<LockFreeRWContention::ReadLock>
                _(buckets_[idx].contention->GetReadLock());
        auto *found = Find(key, idx);
        return iterator(found);
    }

    std::pair<iterator, bool> insert(const value_type &v) {
        size_t idx = HashResult(v.first);
        auto *found = Find(v.first, idx);
        if (found) { return std::make_pair(iterator(found), false); }
        Node *node = node_manager_.GetOne();
        node->Reset(std::move(v));
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention->GetWriteLock());
        return std::make_pair(InsertNode(node), true);
    }

    std::pair<iterator, bool> emplace(Key &&k, Value &&v) {
        size_t idx = HashResult(k);
        auto *found = Find(k, idx);
        if (found) { return std::make_pair(iterator(found), false); }
        auto *node = node_manager_.GetOne();
        node->Reset(std::make_pair(std::move(k), std::move(v)));
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention->GetWriteLock());
        return std::make_pair(InsertNode(node, idx), true);
    }

    size_t erase(const Key &key) {
        size_t idx = HashResult(key);
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention->GetWriteLock());
        return Erase(key, idx);
    }

    size_t erase(iterator &it) {
        if (it == end_) { return 0; }
        size_t idx = HashResult(it->value->first);
        return Erase(it, idx);
    }

    const iterator &end() { return end_; }

 private:
    static bool IsPrime(size_t n) {
        int k = (int)std::sqrt((double)n);
        int i = 2;
        for (; i <= k; ++i) {
            if (n % i == 0) break;
        }
        if (i > k) return true;
        return false;
    }

    static size_t FindAPrime(size_t hint) {
        if (hint >= 500009) return 500009;
        while(!IsPrime(hint)) {
            ++hint;
        }
        return std::min(hint, 500009ul);
    }

    size_t HashResult(const Key &key) const {
        return Hasher()(key) % buckets_.size();
    }

    Node *Find(const Key &key, size_t idx) const {
        // assert(idx < buckets_.size());
        auto *cursor = buckets_[idx].begin.next;
        while (cursor && !Equal()(key, cursor->value->first)) {
            cursor = cursor->next;
        }
        return cursor;
    }

    Node *InsertNode(Node *node, size_t idx) {
        // assert(idx < buckets_.size());
        buckets_[idx].end->next = node;
        node->pred = buckets_[idx].end;
        buckets_[idx].end = node;
        return node;
    }

    size_t Erase(const Key &key, size_t idx) {
        // assert(idx < buckets_.size());
        auto *cursor = Find(key, idx);
        if (!cursor) { return 0; }
        if (cursor == buckets_[idx].end) {
            buckets_[idx].end = cursor->pred;
            cursor->pred.next = nullptr;
        } else {
            cursor->pred.next = cursor->next;
            cursor->next.pred = cursor->pred;
        }
        Reclaim(cursor);
        return 1;
    }

    size_t Erase(iterator &it, size_t idx) {
        // assert(idx < buckets_.size());
        Reclaim(buckets_[idx].DetachNode(it.Inner()));
        return 1;
    }

    void Reclaim(iterator &it) {
        if (!node_manager_.Reclaim(it.Inner())) {
            delete it.Inner();
        }
    }   

    std::vector<Bucket> buckets_;
    iterator end_;
    NodeManager node_manager_;
};
