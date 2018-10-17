#ifndef SERVICE_STATIS_SLIDING_WINDOW
#define SERVICE_STATIS_SLIDING_WINDOW

#include "i_window.h"

template <typename I>
struct state_info
{
unsigned int64_t timestamp;
I state_value;
};

template <typename S>
class sliding_window_state
    : public i_state<S>
{

private:
    unsigned int32_t m_window_size;
    struct state_info<S> *m_fixsized_slots;
    int m_front;
    int m_rear;
public:
    sliding_window_state(unsigned int32_t size):
        m_window_size(size),
        m_fixsized_slots(NULL),
        m_front(0),
        m_rear(0)
    {
        m_fixsized_slots = new state_info[size+1];
        m_front = m_rear = m_fixsized_slots;
    }
    
    ~sliding_window_state()
    {
        delete [] m_fixsized_slots;
        m_fixsized_slots =  NULL:
    }

    bool isEmpty()
    {
        return m_front == m_rear;
    }

    bool isFull()
    {
        return m_front == (m_rear + 1)%m_window_size;
    }

    int push(state_info<S> &item)
    {
        if(isFull())
        {
            return -1;
        }
        m_fixsized_slots[m_rear] = item;
        m_rear = (m_rear + 1)%m_window_size;
    }

    state_info<S> pop()
    {
        if(isEmpty())
        {
            return -1;
        }
        state_info<S> tmp = m_fixed_slots[m_front];
        m_front = (m_front + 1) % m_window_size;
        return tmp;
    }
}; 

template <typename T>
class sliding_window
    : public i_service_window<T>
{

public:
   // sliding_window(

};

#endif
