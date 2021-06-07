#pragma once
#include <atomic>
#include <iostream>

class LockFreeRWContention {
 public:
    LockFreeRWContention() : l_(0), w_(ATOMIC_FLAG_INIT) { }

    class LockFreeRWLock {
     public:
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

    class ReadLock : public LockFreeRWLock {
     public:
        ReadLock(LockFreeRWContention *c) : LockFreeRWLock(), contention_(c) { }
        void lock() { contention_->Read(); }
        void unlock() { contention_->DoneRead(); }

     private:
        LockFreeRWContention *contention_;
    };

    class WriteLock : public LockFreeRWLock {
     public:
        WriteLock(LockFreeRWContention *c) : LockFreeRWLock(), contention_(c) { }
        void lock() { contention_->Write(); }
        void unlock() { contention_->DoneWrite(); }

     private:
        LockFreeRWContention *contention_;
    };

    ReadLock GetReadLock() { return ReadLock(this); }
    WriteLock GetWriteLock() { return WriteLock(this); }

 private:
    void Read() {
        int32_t cnt(0);
        while ((cnt = l_.load(std::memory_order_acquire)) < 0 ||
               !l_.compare_exchange_weak(cnt, cnt + 1,
                                         std::memory_order_release));
    }

    void DoneRead() {
        l_.fetch_sub(1, std::memory_order_release);
    }

    void Write() {
        while(w_.test_and_set(std::memory_order_relaxed));
        int32_t cnt = l_.load(std::memory_order_acquire);
        while(!l_.compare_exchange_weak(cnt, cnt | 0x80000000,
                                        std::memory_order_release));
        while((cnt = l_.load(std::memory_order_acquire)) > 0x80000000);
    }

    void DoneWrite() {
        l_.store(0, std::memory_order_release);
        w_.clear(std::memory_order_relaxed);
    }

    std::atomic<int32_t> l_;
    std::atomic_flag w_;
};