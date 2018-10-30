#ifndef SERVICE_STATIS_I_SERVICE_STATE_H
#define SERVICE_STATIS_I_SERVICE_STATE_H

template <typename T>
class i_state
{

public:
    virtual ~i_state(){}
    virtual void init_state() = 0;
    virtual void update_state(T) = 0;

};

#endif
