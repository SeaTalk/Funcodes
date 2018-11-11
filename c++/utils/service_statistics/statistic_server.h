#ifndef SERVICE_STATIS_STATISTIC_SERVER_H
#define SERVICE_STATIS_STATISTIC_SERVER_H

#include "sliding_window.h"
#include "sync_queue.hpp"

#include <boost/thread.hpp>

template <typename T>
class statistic_server
    : public sliding_window<T>
{
public:
    statistic_server(i_state<T> *state);
    ~statistic_server();

    double get_ok_ratio();
    sync_queue<T> *get_queue();

private:
    sync_queue<T> queue;
    boost::thread t;
    bool run_sign;

    void read_queue_update_window();
};

#endif