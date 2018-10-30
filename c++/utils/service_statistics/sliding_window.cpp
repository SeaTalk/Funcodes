#include "sliding_window.h"
#include <sys/time.h>

template <typename T, typename O>
sliding_window<T,O>::sliding_window(i_state<T> *state)
    :i_service_window(state)
{
    m_window_state->init_state();
}

template <typename T, typename O>
sliding_window<T,O>::~sliding_window()
{

}

template <typename T, typename O>
void sliding_window<T,O>::update_state(T delta_state)
{
    m_window_state->update_state(delta_state);
}

template <typename T, typename O>
i_state<T> *sliding_window<T,O>::get_state()
{
    return m_window_state;
}

template <typename T, typename O>
O sliding_window<T,O>::get_result()
{
    
}