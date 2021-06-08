#pragma once
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <iostream>
#include "rwlock.h"
#include "cache.h"

template<typename T>
struct ValueWrapper {
    ValueWrapper(const std::shared_ptr<T> &v) : value(v) { }
    ValueWrapper(ValueWrapper &&w) {
        value.swap(w.value);
    }
    std::shared_ptr<T> value {nullptr};
    std::atomic<uint64_t> cnt{0};
};

template<class Key, class Value,
         class Map = std::unordered_map<Key, ValueWrapper<Value>>>
class LRUCache : public Cache<Key, Value> {
 public:
    template<typename K, typename V>
    struct LRUCacheListNode {
        const K *key {nullptr};
        ValueWrapper<V> *value {nullptr};
        std::atomic<LRUCacheListNode<K, V>*> next{nullptr};
    };

    LRUCache(size_t capacity) : underly_(std::min(500000ul, std::max(1ul, capacity >> 1))),
                                contention_(), max_size_(capacity), current_size_(0) {
                                    tail_.store(&head_);
                                 }

    std::shared_ptr<Value> Get(const Key &key) {
        std::lock_guard<LockFreeRWContention::ReadLock> _(contention_.GetReadLock());
        auto it = underly_.find(key);
        if (it != underly_.end()) {
            // Detach(&(it->second));
            auto *node = new LRUCacheListNode<Key, Value>();
            node->key = &(it->first);
            node->value = &(it->second);
            node->value->cnt++;
            PutTail(node);
            return it->second.value;
        }
        return nullptr;
    }

    bool Put(const Key &key, const Value &value) {
        std::lock_guard<LockFreeRWContention::WriteLock> _(contention_.GetWriteLock());
        auto res = underly_.emplace(std::make_pair(key, ValueWrapper<Value>(std::make_shared<Value>(value))));
        if (!res.second) { return false; }
        auto &it = res.first;
        auto *node = new LRUCacheListNode<Key, Value>();
        node->key = &(it->first);
        node->value = &(it->second);
        node->value->cnt++;
        PutTail(node);
        if (current_size_ == max_size_) { PopHead(); }
        else { ++current_size_; }
        return true;
    }

    bool Put(const Key &key, std::shared_ptr<Value> value) {
        std::lock_guard<LockFreeRWContention::WriteLock> _(contention_.GetWriteLock());
        auto res = underly_.emplace(std::make_pair(key, ValueWrapper<Value>(value)));
        if (!res.second) { return false; }
        auto &it = res.first;
        auto *node = new LRUCacheListNode<Key, Value>();
        node->key = &(it->first);
        node->value = &(it->second);
        node->value->cnt++;
        PutTail(node);
        if (current_size_ == max_size_) { PopHead(); }
        else { ++current_size_; }
        return true;
    }

 private:
    // void Detach(LRUCacheListNode<Key, Value> *node) {
    //     std::lock_guard<LockFreeRWContention::WriteLock> _p(node->next->cont->GetWriteLock());
    //     std::lock_guard<LockFreeRWContention::WriteLock> _c(node->cont->GetWriteLock());
    //     std::lock_guard<LockFreeRWContention::WriteLock> _n(node->pred->cont->GetWriteLock());
    //     node->pred->next = node->next;
    //     node->next->pred = node->pred;
    // }

    void PutTail(LRUCacheListNode<Key, Value> *node) {
        // if (node != tail_.pred) {
        //     std::lock_guard<LockFreeRWContention::WriteLock> _p(tail_.cont->GetWriteLock());
        //     std::lock_guard<LockFreeRWContention::WriteLock> _c(node->cont->GetWriteLock());
        //     std::lock_guard<LockFreeRWContention::WriteLock> _n(tail_.pred->cont->GetWriteLock());
        //     node->pred = tail_.pred;
        //     tail_.pred = node;
        //     node->next = &tail_;
        //     node->pred->next = node;
        // }
        LRUCacheListNode<Key, Value> *tail(nullptr);
        while (true) {
            tail = tail_.load(std::memory_order_acquire);
            auto *next = tail->next.load(std::memory_order_acquire);
            // std::cout << "put tail:" << tail << ", next:" << next << ", node:" << node <<std::endl;
            if (next) { continue; }
            if (!tail->next.compare_exchange_weak(next, node, std::memory_order_release)) {
                continue;
            }
               // std::cout << "put tail next:" << tail->next.load() << std::endl;
            if (tail_.compare_exchange_strong(tail, node, std::memory_order_release)) {
                break;
            }
        }
        std::cout << "put tail:" << *(node->key) << std::endl;
        // LRUCacheListNode<Key, Value> *tail(nullptr), *dummy(nullptr);
        // do {
        //     tail = tail_.load(std::memory_order_acquire);
        //     dummy = nullptr;
        // } while (!tail->next.compare_exchange_weak(dummy, node, std::memory_order_release));
        // tail_.compare_exchange_strong(tail, node, std::memory_order_release);
    }

    void PopHead() {
        // if (head_.next != &tail_) {
        //     auto *next = head_.next;
        //     head_.next = next->next;
        //     next->next->pred = &head_;
        //     underly_.erase(*(next->key));
        // }
        LRUCacheListNode<Key, Value> *p(nullptr);
        while (true) {
            p = head_.next.load(std::memory_order_acquire);
            if (!p) { return; }
            auto *next = p->next.load(std::memory_order_acquire);
            // std::cout << "p:" << p << ", next:" << next << std::endl;
            // std::cout << "tail:" << tail_.load() << std::endl;
            // std::cout << "emmm: " << tail_.compare_exchange_weak(p, &head_) // &&
            //       << ", mmmmme:" << head_.next.compare_exchange_weak(p, nullptr) << std::endl;
            if (tail_.compare_exchange_weak(p, &head_, std::memory_order_release)) {
                while(!head_.next.compare_exchange_weak(p, nullptr, std::memory_order_release));
            } else {
                // std::cout << "head next:" << head_.next.load() << std::endl;
                while(!head_.next.compare_exchange_strong(p, next, std::memory_order_release));
            }
            // std::cout << "ooo tail:" << tail_.load() << std::endl;
            bool going_to_break(false);
            auto cnt = --(p->value->cnt);
            // std::cout << "cnt:" << cnt << std::endl;
            if (cnt == 0) {
                going_to_break = true;
                underly_.erase(*(p->key));
            }
            std::cout << "delete: " << *(p->key) << std::endl;
            delete p;
            if (going_to_break) { break; }
        }

    }

 private:
    LRUCache(const LRUCache &) = delete;
    LRUCache(LRUCache &&) = delete;
    LRUCache &operator=(const LRUCache &) = delete;
    LRUCache &operator=(LRUCache &&) = delete;

    Map underly_;
    LRUCacheListNode<Key, Value> head_;
    std::atomic<LRUCacheListNode<Key, Value> *> tail_{nullptr};
    LockFreeRWContention contention_;
    size_t max_size_;
    size_t current_size_;
};
