#ifndef SERVICE_STATIS_I_SERVICE_WINDOW_H
#define SERVICE_STATIS_I_SERVICE_WINDOW_H

#include "i_service_state.h"

template <typename T>
class i_service_window
{

public:

    virtual void update_state(T delta_state);
    virtual i_state<T> *get_state();

};

#endif
