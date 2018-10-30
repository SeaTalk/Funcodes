#ifndef SERVICE_STATIS_SLIDING_WINDOW_STATE_H
#define SERVICE_STATIS_SLIDING_WINDOW_STATE_H

#include "i_service_state.h"
#include <unordered_map>

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
    uint32_t m_window_capacity;
    struct state_info<S> *m_fixsized_slots;
    int32_t m_front;
    int32_t m_rear;
    int32_t m_window_size;
    uint32_t m_timeout_ms;
    unordered_map<S, int32_t> m_states_count;
public:
    sliding_window_state(uint32_t size, uint32_t timout):
        m_window_capacity(size),
        m_fixsized_slots(NULL),
        m_front(0),
        m_rear(0),
        m_window_size(0),
        m_timeout_ms(timout),
        m_states_count(2)
    {
        init_state();
    }

    sliding_window_state(uint32_t size):
        m_window_capacity(size),
        m_fixsized_slots(NULL),
        m_front(0),
        m_rear(0),
        m_window_size(0),
        m_timeout_ms(0),
        m_states_count(2)
    {
        init_state();
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

#endif
