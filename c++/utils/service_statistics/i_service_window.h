#ifndef SERVICE_STATIS_I_SERVICE_WINDOW_H
#define SERVICE_STATIS_I_SERVICE_WINDOW_H

#include "i_service_state.h"

template <typename T, typename G>
class i_service_window
{

public:
    virtual i_service_window(i_state<T> window_state);
    virtual ~i_service_window()
    {
        delete m_window_state;
    }

    virtual void update_state(T delta_state);
    virtual i_state<T> *get_state();

    // for user_defined function
    G get_result();

private:
    i_state<T> *m_window_state;



};

#endif
