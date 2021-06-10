#include "lru_cache.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <future>
using namespace std::chrono;
static LRUCache<int, int> cache(10);

void put(int key) {
    std::cout << "put key:" << key << std::endl;
    auto begin = system_clock::now();
    cache.Put(key, key);
    std::cout << "put duration:" << std::chrono::duration_cast<microseconds>(system_clock::now() - begin).count() << std::endl;
}

void get(int key) {
    auto begin = system_clock::now();
    auto v = cache.Get(key);
    if (v) {
        std::cout << "get key:" << key << ", v:" << *v << std::endl;
    } else {
        std::cout << "get key:" << key << ", v: nullptr" << std::endl;
    }
    std::cout << "get duration:" << std::chrono::duration_cast<microseconds>(system_clock::now() - begin).count() << std::endl;
}

void Put() {
    for (int i = 0; i < 200; ++i) {
       std::async(put, i);
    }
}

void Get() {
    for (int i = 0; i < 200; ++i) {
        std::async(get, i);
    }
}

int main() {
    std::thread t1(Put), t2(Get);
    t1.join();
    t2.join();
    return 0;
}
