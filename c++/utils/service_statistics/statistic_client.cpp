#include "statistic_client.h"

template<typename T>
statistic_client<T>::statistic_client(sync_queue<T> *queue)
    : m_queue(queue)
{

}

template<typename T>
int statistic_client<T>::put_service_status(T item)
{
    if (m_queue == nullptr) 
        return -2;
    return m_queue->put(item);
}