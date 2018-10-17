#ifndef SERVICE_STATIS_SLIDING_WINDOW
#define SERVICE_STATIS_SLIDING_WINDOW

#include "i_window.h"

template <typename I>
struct state_info
{
    uint64_t timestamp;
    I state_value;
};

template <typename S>
class sliding_window_state
    : public i_state<S>
{

private:
    unsigned int32_t m_window_capacity;
    struct state_info<S> *m_fixsized_slots;
    int32_t m_front;
    int32_t m_rear;
    int32_t m_window_size;
public:
    sliding_window_state(unsigned int32_t size):
        m_window_capacity(size),
        m_fixsized_slots(NULL),
        m_front(0),
        m_rear(0),
        m_window_size(0);
    {
        m_fixsized_slots = new state_info[size+1];
        m_front = m_rear = 0;
    }
    
    ~sliding_window_state()
    {
        delete [] m_fixsized_slots;
        m_fixsized_slots =  NULL:
    }

    bool isEmpty();
    bool isFull();
    int push(state_info<S> &item);
    state_info<S> pop();
}; 

template <typename T>
class sliding_window
    : public i_service_window<T>
{

public:
   // sliding_window(

};

#endif
