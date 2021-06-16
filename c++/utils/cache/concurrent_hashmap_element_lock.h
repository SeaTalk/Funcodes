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
    typedef NodeManager<value_type, Mutex> NdManager;

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
            : buckets_(FindAPrime(bucket_hint)), end_(nullptr),
              node_manager_(bucket_hint << 1) { }

    iterator find(const Key &key) const {
        size_t idx = HashResult(key);
        auto *found = Find(key, idx);
        return iterator(found);
    }

    std::pair<iterator, bool> insert(const value_type &v) {
        size_t idx = HashResult(v.first);
        auto *found = Find(v.first, idx);
        if (found) { return std::make_pair(iterator(found), false); }
        auto *node = node_manager_.GetOne();
        node->Reset(v);
        return std::make_pair(iterator(InsertNode(node, idx)), true);
    }

    std::pair<iterator, bool> emplace(Key &&k, Value &&v) {
        size_t idx = HashResult(k);
        auto *found = Find(k, idx);
        if (found) { return std::make_pair(iterator(found), false); }
        auto *node = node_manager_.GetOne();
        node->Reset(std::make_pair(std::move(k), std::move(v)));
        return std::make_pair(iterator(InsertNode(node, idx)), true);
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
        auto *cursor = buckets_[idx].begin_.next.load();
        while (cursor) {
            if (cursor->value && Equal()(key, cursor->value->first)) { break; }
            cursor = cursor->next;
        }
        return cursor;
    }

    iterator InsertNode(Node *node, size_t idx) {
        // assert(idx < buckets_.size());
        buckets_[idx].PushBack(node);
        return iterator(node);
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
    NdManager node_manager_;
};
