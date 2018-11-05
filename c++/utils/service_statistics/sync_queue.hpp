#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp> 

template<typename T>
class sync_queue
{
private:
    bool isFull() const
    {
        return m_queue.size() == m_max_size;
    }

    bool isEmpty() const
    {
        return m_queue.empty();
    }

public:
    sync_queue(int maxSize) : m_max_size(maxSize)
    {

    }

    int put(T x)
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        if (isFull())
        {
            return -1;  //队列满了不等待
        }
        m_queue.push_back(x);
        m_not_empty.notify_one();
    }

    void take(T &x)
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        while (isEmpty())
        {
            m_not_empty.wait(m_mutex);
        }
        x = m_queue.m_front();
        m_queue.pop_front();
        m_not_full.notify_one();
    }

private:
    std::list<T> m_queue;   //缓冲区
    boost::mutex m_mutex;   //互斥量和条件变量结合起来使用
    boost::condition_variable_any m_not_empty;  //不为空的条件变量
    boost::condition_variable_any m_not_full;   //没有满的条件变量
    int32_t m_max_size;
};