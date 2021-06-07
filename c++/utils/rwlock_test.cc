#include "rwlock.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using ReadLock = LockFreeRWContention::ReadLock;
using WriteLock = LockFreeRWContention::WriteLock;
LockFreeRWContention contetion;

void read(ReadLock rl) {
    std::lock_guard<ReadLock> _(rl);
    std::cout << "reading..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void write(WriteLock wl) {
    std::lock_guard<WriteLock> _(wl);
    std::cout << "writing..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

void Read() {
    for (int i = 0; i < 200; ++i) {
        read(contetion.GetReadLock());
    }
}

void Write() {
    for (int i = 0; i < 200; ++i) {
        write(contetion.GetWriteLock());
    }
}

int main() {
    std::thread t1(Read), t2(Write);
    t1.join();
    t2.join();
    return 0;
}