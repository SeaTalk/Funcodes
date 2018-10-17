#ifndef I_SERVICE_WINDOW_H
#define I_SERVICE_WINDOW_H

#include "service_state.h"

template <typename T>
class i_service_window
{

public:
    i_service_window(){};
    void set_state(i_state<T> *window_state){this->m_window_state = window_state;}
    i_state<T> *m_window_state;

};

#endif
