#include "lock_free_linked_list.h"
#include <thread>
#include <future>
#include <chrono>
#include <iostream>
using namespace std::chrono;

List<int32_t> list;

void put(int32_t key) {
    auto begin = system_clock::now();
    std::pair<Node<int32_t>*, bool> ret = list.Insert(key);
    std::cout << "put key:" << key << ", success:" << ret.second << std::endl;
    std::cout << "put duration:" << std::chrono::duration_cast<microseconds>(system_clock::now() - begin).count() << std::endl;
}

void get(int32_t key) {
    auto begin = system_clock::now();
    Node<int32_t> *v = list.Search(key);
    if (v) {
        std::cout << "get key:" << key << ", v:" << *(v->value) << std::endl;
    } else {
        std::cout << "get key:" << key << ", v: nullptr" << std::endl;
    }
}

void Put() {
    for (int i = 0; i < 100; ++i) {
        std::async(std::launch::async, put, i);
    }
}

void Get() {
    for (int i = 0; i < 100; ++i) {
        std::async(std::launch::async, get, i);
    }
}


int main() {
    std::thread t1(Put);
    std::thread t3(Put);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::thread t2(Get);
    t1.join();
    t2.join();
    t3.join();

    for (auto it = list.Begin(); it != list.End(); it = it->Next()) {
        std::cout << "v:" << *(it->value) << std::endl;
    }

    return 0;
}