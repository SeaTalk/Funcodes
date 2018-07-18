#include<iostream>
using namespace std;

class Father
{
public:
    virtual void print(){ cout << "Father"<<endl; }
};

class Son : public Father
{
public:
    void print(){ cout << "Son" <<endl; }
};

int main()
{
    Father *f = new Son();

    // What's the result when f call print() ?
    f->print();
}
