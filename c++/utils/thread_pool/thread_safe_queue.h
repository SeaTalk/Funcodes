#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {

 public:
    ThreadSafeQueue() {}

    void push(const T &item) {
        std::lock_guard<std::mutex> l(mut_);
        data_queue_.push(item);
        cond_.notify_one();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> l(mut_);
        cond_.wait(l, [this](){ return !data_queue_.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
        data_queue_.pop();
        return res;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> l(mut_);
        cond_.wait(l, [this](){ return !data_queue_.empty(); });
        value = data_queue_.front();
        data_queue_.pop();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> l(mut_);
        if (data_queue_.empty()) {
            return false;
        }
        value = data_queue_.front();
        data_queue_.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> l(mut_);
        if (data_queue_.empty()) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
        data_queue_.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> l(mut_);
        return data_queue_.empty();
    }

 private:
    // disable copy
    ThreadSafeQueue(const ThreadSafeQueue &other) = delete;
    ThreadSafeQueue(ThreadSafeQueue &other) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue &other) = delete;

    mutable std::mutex mut_;
    std::queue<T> data_queue_;
    std::condition_variable cond_;
};