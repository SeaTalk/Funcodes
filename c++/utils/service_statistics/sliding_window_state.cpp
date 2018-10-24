#include "sliding_window_state.h"
#include <sys/time.h>

template <typename T>
bool sliding_window_state<T>::isEmpty()
{
    return m_front == m_rear;
}

template <typename T>
bool sliding_window_state<T>::isFull()
{
    return m_front == (m_rear + 1)%m_window_capacity;
}

template <typename T>
int sliding_window_state<T>::push(state_info<T> &item)
{
    if(isFull())
    {
        return -1;
    }
    m_fixed_slots[m_rear] = item;
    m_rear = (m_rear + 1) % m_window_capacity;
    ++m_window_size;
    return 0;
}

template <typename T>
state_info<T> sliding_window_state<T>::pop()
{
    if(isEmpty())
    {
        return -1;
    }
    state_info<T> tmp = m_fixed_slots[m_front];
    m_front = (m_front + 1) % m_window_capacity;
    --m_window_size;
    return tmp;
}

template <typename T>
void sliding_window_state<T>::init_state()
{
    m_fixed_slots = new state_info[size+1];
    m_front = m_rear = 0;
}

template <typename T>
void sliding_window_state<T>::update_state(T value) 
{
    // firstly remove outdated items in the ring queue
    if (!isEmpty())
    {
        uint64_t time_line = m_fixed_slots[m_front].timestamp - m_timeout_ms;
        while(!isEmpty() && m_fixed_slots[m_rear].timestamp < time_line)
        {
            pop();
        }
    }
    if (isFull())
    {
        pop();
    }

    // secondly insert new item into ring
    struct timeval tv;
    struct state_info si;
    gettimeofday(&tv,NULL);
    si.timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000; 
    si.state_value = value;
    push(si);
}


