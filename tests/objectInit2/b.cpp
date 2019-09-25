#include "b.h"
#include <iostream>
using namespace std;

B::B()
{
    a=NULL;
    cout<<"B construction"<<endl;
}

B::~B()
{
    if (a != NULL)
        delete a;
    cout<<"B destruction"<<endl;
}
