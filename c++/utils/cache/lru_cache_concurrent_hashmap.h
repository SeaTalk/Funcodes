#pragma once
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include "cache.h"
#include "spinlock.h"
#include "concurrent_hashlist.h"
#include "concurrent_hashmap_element_lock.h"

template<class Key, class Value, class Mutex = SpinMutex,
         class Hasher = std::hash<Key>,
         class Equal = std::equal_to<Key>,
         class Pair = std::pair<Key*, Value>,
         class MValue = ConcurrentHashListNode<Pair, Mutex>,
         class Map = ConcurrentHashMap<Key, MValue, Mutex, Hasher, Equal> >
class LRUCache : public Cache<Key, Value> {
 public:
    using List = ConcurrentHashList<Pair, Mutex>;
    LRUCache(size_t capacity, uint64_t timeout_in_second = NEVER_TIMEOUT) :
            underly_(std::min(500000ul, std::max(1ul, capacity >> 1))),
            list_(), max_size_(capacity), current_size_(0), timeout_(timeout_in_second) {
    }

    Value *Get(const Key &key) {
        auto now = std::time(nullptr);
        auto it = underly_.find(key);
        if (it == underly_.end()) { return nullptr; }
        if (list_.DetachNode(&(it->second))) {
            if (it->second.expire_time > now) {
                underly_.erase(it);
                return nullptr;
            }
            list_.PushBack(&(it->second));
        }
        if (it->seond.value) {
            return &(it->second.value->second);
        }
        return nullptr;
    }

    bool Put(const Key &key, const Value &value) {
        auto res = underly_.emplace(std::make_pair(key, MValue()));
        if (!res.second) { return false; }
        auto &it = res.first;
        it->second.Reset(std::make_pair((&it->first), value));
        it->second.timeout = std::time(nullptr) + timeout_;
        list_.PushBack(&(it->second));
        if (++current_size_ >= max_size_) {
            auto *head = list_.PopHead();
            underly_.erase(*(head->value->first));
        }
        return true;
    }

 private:
    Map underly_;
    List list_; 
    size_t max_size_;
    size_t current_size_;
    uint64_t timeout_;
};
