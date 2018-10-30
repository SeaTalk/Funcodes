#include "sliding_window.h"
#include <sys/time.h>

template <typename T>
sliding_window<T>::sliding_window(i_state<T> *state)
{
    m_window_state->init_state();
}

template <typename T>
sliding_window<T>::~sliding_window()
{
    delete m_window_state;
}

template <typename T>
void sliding_window<T>::update_state(T delta_state)
{
    m_window_state->update_state(delta_state);
}

template <typename T>
i_state<T> *sliding_window<T>::get_state()
{
    return m_window_state;
}
