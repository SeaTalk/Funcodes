#include "statistic_server.h"

template <typename T>
statistic_server<T>::statistic_server(i_state<T> *state)
    : sliding_window<T>(state)
{
    t = boost::thread(&statistic_server<T>::read_queue_update_window, this);
}

template <typename T>
statistic_server<T>::~statistic_server()
{
    t.join();
}

template <typename T>
double statistic_server<T>::get_ok_ratio()
{
#define STATE_OK 1
    sliding_window_state *state = (sliding_window_state *) sliding_window::m_window_state;
    if (state->m_window_size == 0)
    {
        return 0;
    }
    return state->m_states_count[STATE_OK] * 0.1 / state->m_window_size;
}

template <typename T>
sync_queue<T> * statistic_server<T>::get_queue()
{
    return &queue;
}

template <typename T>
void statistic_server<T>::read_queue_update_window()
{
    T value;
    queue.take(value);
    sliding_window::update_state(value);
}