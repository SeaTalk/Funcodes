#include "a.h"

#include <iostream>
using namespace std;

A::A()
{
    tmp = new int[5];
    cout << "A constructs a array of 5 int"<<endl;
}

A::~A()
{
    delete tmp;
    cout << "A destructs and release space of tmp"<<endl;
}


