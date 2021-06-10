#pragma once
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include "rwlock.h"
#include "cache.h"

#define NEVER_TIMEOUT 0x8000000000000000

template<typename K, typename V>
struct LRUCacheListNode {
    std::shared_ptr<V> value = nullptr;
    const K *key = nullptr;
    LRUCacheListNode<K, V> *pred = nullptr;
    LRUCacheListNode<K, V> *next = nullptr;
    std::unique_ptr<LockFreeRWContention> cont = nullptr;
    uint64_t expire_time = NEVER_TIMEOUT;
};

template<class Key, class Value,
         class Map = std::unordered_map<Key, LRUCacheListNode<Key, Value>>>
class LRUCache : public Cache<Key, Value> {
 public:
    LRUCache(size_t capacity, uint64_t timeout_in_second = NEVER_TIMEOUT) :
            underly_(std::min(500000ul, std::max(1ul, capacity >> 1))),
            contention_(), max_size_(capacity), current_size_(0), timeout_(timeout_in_second) {
        tail_.pred = &head_;
        head_.next = &tail_;
        head_.cont.reset(new LockFreeRWContention());
        tail_.cont.reset(new LockFreeRWContention());
    }

    std::shared_ptr<Value> Get(const Key &key) {
        std::lock_guard<LockFreeRWContention::ReadLock> _(contention_.GetReadLock());
        auto now = std::time(nullptr);
        auto it = underly_.find(key);
        if (it != underly_.end() && it->second.expire_time < now) {
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
        it->second.expire_time = std::time(nullptr) + timeout_;
        PutTail(&(it->second));
        if (++current_size_ >= max_size_) { PopHead(); }
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
        it->second.expire_time = std::time(nullptr) + timeout_;
        PutTail(&(it->second));
        if (++current_size_ >= max_size_) { PopHead(); }
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

        node->expire_time = std::time(nullptr) + timeout_;
    }

    void PopHead() {
        if (head_.next != &tail_) {
            auto now = std::time(nullptr);
            auto *next = head_.next->next;
            while(next->expire_time < now) {
                next = next->next;
            }
            auto *begin = head_.next;
            head_.next = next;
            next->pred = &head_;
            while (begin != next) {
                auto *bn = begin->next;
                underly_.erase(*(begin->key));
                begin = bn;
            }
        }
    }

 private:
    Map underly_;
    LRUCacheListNode<Key, Value> head_;
    LRUCacheListNode<Key, Value> tail_;
    LockFreeRWContention contention_;
    size_t max_size_;
    size_t current_size_;
    uint64_t timeout_;
};
