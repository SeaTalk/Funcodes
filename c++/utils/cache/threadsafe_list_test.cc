#include "threadsafe_listed_queue.h"
#include <future>
#include <thread>
#include "thread_pool.h"

ListedQueue<int> queue;
ThreadPool pool(8);

void Put(int i) {
    auto *node = new ListedQueueNode<int>(i);
    queue.PushBack(node);
}

void Get() {
    auto *p = queue.PopHead();
    if (p) {
        delete p;
    }
}

void put() {
    for (int i = 0; i < 100; ++i) {
        pool.Submit([i]{
            Put(i);
        });
    }
}

void put2() {
    for (int i = 0; i < 100; ++i) {
        pool.Submit([i]{
            Put(i + 100);
        });
    }
}

void get() {
    for (int i = 0; i < 100; ++i) {
        pool.Submit([]{
            Get();
        });
    }
}

int main() {
    std::thread t1(put), t2(get), t3(put2);
    t1.join();
    t2.join();
    t3.join();
    while(!pool.Empty());
    return 0;
}