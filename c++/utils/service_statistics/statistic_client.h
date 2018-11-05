#ifndef SERVICE_STATIS_STATISTIC_CLIENT_H
#define SERVICE_STATIS_STATISTIC_CLIENT_H

#include "sync_queue.hpp"

template <typename T>
class statistic_client
{
public:
    statistic_client(sync_queue<T> *queue);
    int put_service_status(T item);
private:
    sync_queue<T> *m_queue;
    bool avaiable;
};

#endif