#include "lru_cache.h"
#include <thread>
#include <iostream>

static LRUCache<int, int> cache(10);

void put(int key) {
    std::cout << "put key:" << key << std::endl;
    cache.Put(key, key);
}

void get(int key) {
    auto v = cache.Get(key);
    if (v) {
        std::cout << "get key:" << key << ", v:" << *v << std::endl;
    } else {
        std::cout << "get key:" << key << ", v: nullptr" << std::endl;
    }
}

void Put() {
    for (int i = 0; i < 200; ++i) {
       put(i);
    }
}

void Get() {
    for (int i = 0; i < 200; ++i) {
        get(i);
    }
}

int main() {
    std::thread t1(Put), t2(Get);
    t1.join();
    t2.join();
    return 0;
}