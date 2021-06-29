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
template<typename T>
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
    ConcurrentHashListNode<T> *pred {nullptr};
    ConcurrentHashListNode<T> *next {nullptr};
    long long timeout {NEVER_TIMEOUT};
};

template<typename T, typename Mutex = SpinMutex>
struct ConcurrentHashList {
    typedef ConcurrentHashListNode<T> Node;
    ConcurrentHashList(bool delete_node = true) :
        begin(), end(&begin), need_del(delete_node), contention() { }

    ~ConcurrentHashList() {
        if (!need_del) { return; }
        Node *cur{nullptr};
        while((cur = PopHead())) {  delete cur; }
    }

    bool PushBack(Node *node) {
        std::lock_guard<Mutex> _(contention);
        if (end != node) {
            end->next = node;
            node->pred = end;
            node->next = nullptr;
            end = node;
        }
        return true;
    }

    Node *PopHead() {
        std::lock_guard<Mutex> _(contention);
        if (end == &begin) { return nullptr; }
        auto *cur = begin.next;
        if (cur == end) {
            end = end->pred;
            end->next = nullptr;
        } else {
            begin.next = cur->next;
            cur->next->pred = &begin;
        }
        return cur;
    }

    bool DetachNode(Node *node) {
        std::lock_guard<Mutex> _(contention);
        if (node == end) {
            end = end->pred;
            end->next = nullptr;
        } else {
            node->pred->next = node->next;
            node->next->pred = node->pred;
        }
        return true;
    }

    Node begin;
    Node *end;
    bool need_del;
    mutable Mutex contention;
};

template<typename T, typename Mutex = SpinMutex>
class TNodeManager {
public:
    using List = ConcurrentHashList<T, Mutex>;
    using Node = typename List::Node;
    TNodeManager(size_t window_s = 1, size_t slide_s = 1)
            : window_in_seconds_(window_s), slide_in_seconds_(slide_s),
              reclaim_thread_(ReuseLoop, this) { }

    ~TNodeManager() {
        running_ = false;
        reclaim_thread_.join();
    }

    Node *GetOne() override {
        Node *ret = this->resouce_.PopHead();
        if (!ret) {
            ret = new Node();
        } else {
            ret->next = nullptr;
        }
        return ret;
    }

    bool Reclaim(Node *node) override {
        node->timeout = time(0) + this->window_in_seconds_;
        return this->running_ && this->reclaimer_.PushBack(node);
    }

    bool Reuse() override {
        auto expired_time = time(0);
        Node *cur {this->reclaimer_.begin.next}, *last{&this->reclaimer_.begin};
        do {
            if (last == this->reclaimer_.end ||
                cur->timeout > expired_time) { break; }
            last = cur;
            cur = cur->next;
        } while (true);
        if (last == &this->reclaimer_.begin) { return false; }
        do {
            std::lock_guard<Mutex> _(this->reclaimer_.contention);
            cur = this->reclaimer_.begin.next;
            if (last == this->reclaimer_.end) {
                this->reclaimer_.end = &(this->reclaimer_.begin);
                this->reclaimer_.begin.next = nullptr;
            } else {
                this->reclaimer_.begin.next = last->next;
                last->next->pred = &this->reclaimer_.begin;
            }
        } while(0);
        do {
            std::lock_guard<Mutex> _(this->resouce_.contention);
            this->resouce_.end->next = cur;
            cur->pred = this->resouce_.end;
            last->next = nullptr;
            this->resouce_.end = last;
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
