#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <random>

template<typename T>
class ThreadSafeQueue {
 public:
    ThreadSafeQueue() {}
    ThreadSafeQueue(ThreadSafeQueue const& other) {
        std::lock_guard<std::mutex> lk(other.mu_);
        data_queue_=other.data_queue_;
    }

    void Push(T new_value) {
        std::lock_guard<std::mutex> lk(mu_);
        data_queue_.push(new_value);
        data_cond_.notify_one();
    }

    void WaitAndPop(T& value) {
        std::unique_lock<std::mutex> lk(mu_);
        data_cond_.wait(lk, [this]{ return !running_ || !data_queue_.empty(); });
        if (running_) {
            value = std::move(data_queue_.front());
            data_queue_.pop();
        }
    }

    bool TryPop(T& value) {
        std::lock_guard<std::mutex> lk(mu_);
        if(data_queue_.empty())
            return false;
        value = std::move(data_queue_.front());
        data_queue_.pop();
        return true;
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lk(mu_);
        return data_queue_.empty();
    }

    void Close() {
        running_ = false;
        data_cond_.notify_all();
    }

 private:
    mutable std::mutex mu_;
    std::queue<T> data_queue_;
    std::condition_variable data_cond_;
    bool running_ = true;
};

class ThreadJoiner {
 public:
    explicit ThreadJoiner(std::vector<std::thread> &t): threads_(t) {}
    ~ThreadJoiner() {
        for (auto &t : threads_) {
            if(t.joinable()) { t.join(); }
        }
    }

    ThreadJoiner(ThreadJoiner const&)=delete;
    ThreadJoiner& operator=(ThreadJoiner const&)=delete;

 private:
    std::vector<std::thread> &threads_;
};

class ThreadPool {
 public:
    struct ThreadPoolCreateException : public std::exception {
        const char* what() const throw() {
            return "create thread pool exception.";
        }
    };

    explicit ThreadPool(unsigned thread_count) :
            ThreadPool(thread_count, "") {}

    ThreadPool(unsigned thread_count, const std::string& name) :
            name_(name), done_(false),
            work_queues_(thread_count), joiner_(threads_), u(0, thread_count) {
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads_.emplace_back(std::thread(&ThreadPool::Run, this, i));
            }
            //  use notifier
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } catch(...) {
            done_ = true;
            throw ThreadPoolCreateException();
        }
    }

    ~ThreadPool() { 
        done_ = true;
        for (auto &q : work_queues_) { q.Close(); }
    }

    void Submit(std::function<void()> f) {
        std::default_random_engine e;
        work_queues_[u(e)].Push(std::move(f));
        // index = index % threads_.size();
    }

    bool Empty() {
        int ret = 0;
        std::for_each(work_queues_.begin(), work_queues_.end(), [&](const ThreadSafeQueue<std::function<void()> > &q){
            ret += (!q.Empty());
        });
        return ret == 0;
    }

 private:
    void Run(int index) {
        // if (!name_.empty()) { prctl(PR_SET_NAME, name_.c_str()); }
        ThreadSafeQueue<std::function<void()> > &work_queue
                = work_queues_[index];
        while(!done_) {
            std::function<void()> task([](){});
            work_queue.WaitAndPop(task);
            task();
        }
    }

    std::string name_;
    std::atomic_bool done_;
    std::vector<ThreadSafeQueue<std::function<void()> > > work_queues_;
    std::vector<std::thread> threads_;
    ThreadJoiner joiner_;
    std::uniform_int_distribution<unsigned> u;
};
