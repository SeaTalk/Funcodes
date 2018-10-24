#ifndef SERVICE_STATIS_I_SERVICE_WINDOW_H
#define SERVICE_STATIS_I_SERVICE_WINDOW_H

#include "i_service_state.h"

template <typename T, typename G>
class i_service_window
{

public:
    i_service_window(i_state<T> window_state){};
    i_state<T> *m_window_state;

    // for user_defined function
    G get_result();

};

#endif
