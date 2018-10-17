#ifndef I_SERVICE_STATE_H
#define I_SERVICE_STATE_H

template <typename T>
class i_state{

public:
    virtual ~i_state(){}
    virtual void init_state() = 0;
    virtual void update_state() = 0;
    virtual T get_state() = 0;

};

#endif
