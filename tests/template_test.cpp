#include <iostream>
using namespace std;

class base
{
public:
    base(){cout<<"this is base"<<endl;}
};

template <typename T>
class new_class : public base
{
public:
    new_class(T a){cout<<"this is new :"<<a <<endl;}
};

int main()
{
    base* b = new new_class<int>(5);
    delete b;
}
