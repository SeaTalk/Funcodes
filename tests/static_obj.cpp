#include <iostream>
using namespace std;

class A
{
public:
    A(int t=1){cout<<"A's constructor."<<endl;num=t;}
    ~A(){cout<<"A's destructor"<<endl;}
    int get_num() {cout << "get_num"<<endl; return num;}
private:
    int num;
};

class B
{
public:
    B():tmp(a.get_num()){cout<<"B's constructor"<<endl;}
    ~B(){cout<<"B's destructor"<<endl;}
    void print_tmp(){cout<<tmp<<endl;}
    static A a;
private:
    int tmp;
};

/**
 * NOTICE: different with "A B::a = A(2);" 
 * if class A is non-copyable ( A has con/destructors privated etc. )
 * we have to initialize static object 'a' like below. 
 */  
A B::a(2); 

int main()
{
    B b;
    b.print_tmp();
}
