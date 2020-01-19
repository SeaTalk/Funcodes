#include "thread_safe_queue.h"
#include <thread>
#include <vector>
#include <functional>

class ThreadPool {
    struct JoinThreads {
     private:
        std::vector<std::thread> &threads_;

     public:
        explicit JoinThreads(std::vector<std::thread> &threads) : threads_(threads){}
        ~JoinThreads() {
            for (size_t i = 0; i < threads_.size(); ++i) {
                if (threads_[i].joinable()) {
                    threads_[i].join();
                }
            }
        }
    };
 public:
    ThreadPool() : done_(false), joiner_(threads_) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try {
            for (size_t i = 0; i < thread_count; ++i) {
                threads_.emplace_back(&ThreadPool::work, this);
            }
        } catch (...) {
            done_ = true;
            throw;
        }
    }
    ~ThreadPool() {
        done_ = true;
    }

    template<typename FunctionType>
    void submit(FunctionType f) {
        work_queue_.push(std::function<void()> (f));
    }

 private:
    void work() {
        while(!done_) {
            std::function<void()> task;
            if (work_queue_.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

    std::atomic_bool done_;
    ThreadSafeQueue<std::function<void()> > work_queue_;
    std::vector<std::thread> threads_;
    JoinThreads joiner_;

};