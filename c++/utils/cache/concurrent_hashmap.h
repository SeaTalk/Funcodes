#pragma once
#include <cmath>
#include <memory>
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
        InnerListNode(const T &v) : value(v) { }
        InnerListNode(T &&v) : value(std::forward<T>(v)) { }
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

    ConcurrentHashMap(size_t bucket_hint=3) : bucktes_(FindAPrime(bucket_hint)) { }

    iterator find(const Key &key) const {
        size_t idx = HashResult(key);
        std::lock_guard<LockFreeRWContention::ReadLock>
                _(buckets_[idx].contention.GetReadLock());
        auto *found = Find(key, idx);
        return found ? &found->value, nullptr;
    }

    iterator insert(const value_type &)

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
        return std::min(hint, 500009);
    }

    static size_t HashResult(const Key &key) {
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

    std::vector<Bucket<value_type>> buckets_;
    value_type *end_ {nullptr};
};
