#ifndef SERVICE_STATIS_SLIDING_WINDOW
#define SERVICE_STATIS_SLIDING_WINDOW

#include "i_service_window.h"

template <typename T>
class sliding_window
    : public i_service_window<T>
{

public:
    sliding_window(i_state<T> *state);
    virtual ~sliding_window();
    void update_state(T delta_state);
    i_state<T> *get_state();

protected:
    i_state<T> *m_window_state;
};

#endif
