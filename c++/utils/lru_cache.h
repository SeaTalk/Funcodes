#pragma once
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include "rwlock.h"

template<typename K, typename V>
struct LRUCacheListNode {
    std::shared_ptr<V> value = nullptr;
    const K *key = nullptr;
    LRUCacheListNode<K, V> *pred = nullptr;
    LRUCacheListNode<K, V> *next = nullptr;
    std::shared_ptr<LockFreeRWContention> cont = nullptr;
};

template<class Key, class Value,
         class Map = std::unordered_map<Key, LRUCacheListNode<Key, Value>>>
class LRUCache {
 public:
    LRUCache(size_t capacity) : underly_(std::min(500000ul, std::max(1ul, capacity >> 1))),
                                contention_(), max_size_(capacity), current_size_(0) {
        tail_.pred = &head_;
        head_.next = &tail_;
        head_.cont.reset(new LockFreeRWContention());
        tail_.cont.reset(new LockFreeRWContention());
    }

    std::shared_ptr<Value> Get(const Key &key) {
        std::lock_guard<LockFreeRWContention::ReadLock> _(contention_.GetReadLock());
        auto it = underly_.find(key);
        if (it != underly_.end()) {
            PutTail(&(it->second));
            return it->second.value;
        }
        return nullptr;
    }

    bool Put(const Key &key, const Value &value) {
        std::lock_guard<LockFreeRWContention::WriteLock> _(contention_.GetWriteLock());
        auto res = underly_.emplace(std::make_pair(key, LRUCacheListNode<Key, Value>()));
        if (!res.second) { return false; }
        auto &it = res.first;
        it->second.key = &(it->first);
        it->second.value = std::make_shared<Value>(value);
        it->second.cont.reset(new LockFreeRWContention());
        PutTail(&(it->second));
        if (current_size_ == max_size_) { PopHead(); }
        else { ++current_size_; }
        return true;
    }

    bool Put(const Key &key, std::shared_ptr<Value> value) {
        std::lock_guard<LockFreeRWContention::WriteLock> _(contention_.GetWriteLock());
        auto res = underly_.emplace(std::make_pair(key, LRUCacheListNode<Key, Value>()));
        if (!res.second) { return false; }
        auto &it = res.first;
        it->second.key = &(it->first);
        it->second.value = value;
        it->second.cont.reset(new LockFreeRWContention());
        PutTail(&(it->second));
        if (current_size_ == max_size_) { PopHead(key); }
        else { ++current_size_; }
        return true;
    }

 private:
    void PutTail(LRUCacheListNode<Key, Value> *node) {
        if (node->pred) {
            std::lock_guard<LockFreeRWContention::WriteLock> _p(node->next->cont->GetWriteLock());
            std::lock_guard<LockFreeRWContention::WriteLock> _c(node->cont->GetWriteLock());
            std::lock_guard<LockFreeRWContention::WriteLock> _n(node->pred->cont->GetWriteLock());
            node->pred->next = node->next;
            node->next->pred = node->pred;
        }

        if (node != tail_.pred) {
            std::lock_guard<LockFreeRWContention::WriteLock> _p(tail_.cont->GetWriteLock());
            std::lock_guard<LockFreeRWContention::WriteLock> _c(node->cont->GetWriteLock());
            std::lock_guard<LockFreeRWContention::WriteLock> _n(tail_.pred->cont->GetWriteLock());
            node->pred = tail_.pred;
            tail_.pred = node;
            node->next = &tail_;
            node->pred->next = node;
        }
    }

    void PopHead() {
        if (head_.next != &tail_) {
            auto *next = head_.next;
            head_.next = next->next;
            next->next->pred = &head_;
            underly_.erase(*(next->key));
        }
    }

 private:
    Map underly_;
    LRUCacheListNode<Key, Value> head_;
    LRUCacheListNode<Key, Value> tail_;
    LockFreeRWContention contention_;
    size_t max_size_;
    size_t current_size_;
};
