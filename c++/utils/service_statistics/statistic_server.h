#ifndef SERVICE_STATIS_STATISTIC_SERVER_H
#define SERVICE_STATIS_STATISTIC_SERVER_H

#include "sliding_window.h"

template <typename T>
class statistic_server
    : public sliding_window<T>
{
public:
    statistic_server(i_state<T> *state);

    double get_ok_ratio();
};

#endif