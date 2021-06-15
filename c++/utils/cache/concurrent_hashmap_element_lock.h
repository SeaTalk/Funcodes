#pragma once
#include <cmath>
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>
#include <utility>
#include "spinlock.h"
#include "concurrent_hashlist.h"

template<class Key, class Value,
         class Mutex = SpinMutex,
         class Hasher = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class ConcurrentHashMap {
 public:
    typedef std::pair<Key, Value> value_type;
    typedef ConcurrentHashList<value_type, Mutex> Bucket;
    typedef ConcurrentHashListNode<value_type, Mutex> Node;
    typedef NodeManager<value_type, Mutex> NodeManager;

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

    ConcurrentHashMap(size_t bucket_hint=3)
            : buckets_(FindAPrime(bucket_hint)), end_(nullptr) { }

    iterator find(const Key &key) const {
        size_t idx = HashResult(key);
        auto *found = Find(key, idx);
        return iterator(found);
    }

    iterator insert(const value_type &v) {
        size_t idx = HashResult(v.first);
        auto *node = node_manager_.GetOne();
        node->Reset(v);
        return InsertNode(node);
    }

    iterator emplace(Key &&k, Value &&v) {
        size_t idx = HashResult(k);
        auto *node = node_manager_.GetOne();
        node->Reset(std::make_pair(std::forward(k), std::forward(v)));
        return InsertNode(node);
    }

    size_t erase(const Key &key) {
        size_t idx = HashResult(key);
        return Erase(key, idx);
    }

    size_t erase(iterator &it) {
        if (it == end_) { return 0; }
        size_t idx = HashResult(it->value->first);
        return Erase(it, idx);
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

    Node *Find(const Key &key, size_t idx) const {
        assert(idx < buckets_.size());
        auto *cursor = buckets_[idx].next;
        while (!cursor && !Equal()(key, cursor->value.first)) {
            cursor = cursor->next;
        }
        return cursor;
    }

    iterator InsertNode(Node *node, size_t idx) {
        assert(idx < buckets_.size());
        buckets_[idx].PushBack(node);
        return iterator(node);
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

    size_t Erase(iterator &it, size_t idx) {
        assert(idx < buckets_.size());
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
