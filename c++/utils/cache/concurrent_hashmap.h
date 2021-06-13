#pragma once
#include <cmath>
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>
#include <utility>
#include "rwlock.h"

template<class Key, class Value,
         class Hasher = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class ConcurrentHashMap {
 public:
    template<typename T>
    struct InnerListNode {
        InnerListNode() { }
        template<typename ...Args>
        InnerListNode(Args&&... args) : value(new T(std::forward<Args>(args)...)) { }
        std::unique_ptr<T> value;
        InnerListNode<T> *pred {nullptr};
        InnerListNode<T> *next {nullptr};
    };

    template<typename T>
    struct Bucket {
        Bucket() : contention(new LockFreeRWContention()), begin(), end(&begin) { }
        std::unique_ptr<LockFreeRWContention> contention;
        InnerListNode<T> begin;
        InnerListNode<T> *end;
    };

    typedef std::pair<Key, Value> value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    ConcurrentHashMap(size_t bucket_hint=3) : buckets_(FindAPrime(bucket_hint)) { }

    iterator find(const Key &key) const {
        size_t idx = HashResult(key);
        std::lock_guard<LockFreeRWContention::ReadLock>
                _(buckets_[idx].contention.GetReadLock());
        auto *found = Find(key, idx);
        return found ? &found->value : end_;
    }

    iterator insert(const value_type &v) {
        size_t idx = HashResult(v.first);
        InnerListNode<value_type> *node =
                new InnerListNode<value_type>(std::move(v));
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention.GetWriteLock());
        return InsertNode(node);
    }

    iterator emplace(Key &&k, Value &&v) {
        size_t idx = HashResult(k);
        auto *node = new InnerListNode<value_type>(
                std::make_pair(std::forward(k), std::forward(v)));
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention.GetWriteLock());
        return InsertNode(node);
    }

    size_t erase(const Key &key) {
        size_t idx = HashResult(key);
        std::lock_guard<LockFreeRWContention::WriteLock>
                _(buckets_[idx].contention.GetWriteLock());
        return Erase(key, idx);
    }

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

    size_t HashResult(const Key &key) {
        return Hasher(key) % buckets_.size();
    }

    InnerListNode<value_type> *Find(const Key &key, size_t idx) const {
        assert(idx < buckets_.size());
        auto *cursor = buckets_[idx].next;
        while (!cursor && !Equal(key, cursor->value.first)) {
            cursor = cursor->next;
        }
        return cursor;
    }

    iterator InsertNode(InnerListNode<value_type> *node, size_t idx) {
        assert(idx < buckets_.size());
        buckets_[idx].end->next = node;
        node->pred = buckets_[idx].end;
        buckets_[idx].end = node;
        return node->value.get();
    }

    size_t Erase(const Key &key, size_t idx) {
        assert(idx < buckets_.size());
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

    void Reclaim(iterator it) {
        delete it;
    }

    std::vector<Bucket<value_type>> buckets_;
    value_type *end_ {nullptr};
};
