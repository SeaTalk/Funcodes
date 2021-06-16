#pragma once
#include <atomic>

class SpinMutex {
 public:
    SpinMutex() : m_(false) { }

    void lock() {
        while(m_.test_and_set(std::memory_order_acquire));
    }

    bool try_lock() {
        if (m_.test_and_set(std::memory_order_acquire)) {
            return false;
        }
        return true;
    }

    void unlock() {
        m_.clear(std::memory_order_release);
    }
 private:
    std::atomic_flag m_;
};