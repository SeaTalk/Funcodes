#include<iostream>
using namespace std;

/*
 * This test is exploring the use of virtual destructor.
 * Constructor is used usually to build an object 
 * Meanwhile, destructor is to delete an object 
 *
 * Then why we need destructor of base class to be virtual ?
 *
 */

class base
{
public:
    base(){}
    virtual void print(){cout<<"bast print"<<endl;}
    virtual ~base(){cout << "this is base destructor"<<endl;}
};

class ac : public base
{
public:
    void print()
    {
        cout<<"ac print"<<endl;
        cs = new char[5];
        cout << "cs is a pointer of char"<<endl;
    }
private:
    char* cs;
    ~ac()
    {
        cout<<"this is ac destructor"<<endl;
        delete cs; 
    }
};

int main()
{
    base *b = new ac;
    b->print();
    delete b;
}

/**
 *OUTPUT:
 *  ac print
 *  cs is a pointer of char
 *  this is ac destructor
 *  this is base destructor
 *
 * If the base destructor is not virtual, the ac destructor would NOT be called at all
 */
