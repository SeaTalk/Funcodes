#include <iostream>
#include <string>
using namespace std;

class Father
{
public:
    void f1(int a){cout<<"father func1 with int parameter:"<<a<<endl;}
    void f1(string &s){cout<<"father func1 with string parameter:"<<s<<endl;}
    virtual void f1(double d){cout<<"father func1 with double parameter:"<<d<<endl;}
};

class Son: public Father
{
public:
    void f1(string &s){Father::f1(s);cout<<"son func1 with string:"<<s<<endl;}
    void f1(double d){Father::f1(d);cout<<"son func1 with double:"<<d<<endl;}                                                     
    void f1(int a){cout<<"son func1 with int :"<<a<<endl;Father::f1(a+1);}
};

int main()
{
    Son s;
    string str("Hello");
    s.f1(0);
    s.f1(2.3);
    s.f1(str);
}

/**
 * OUTPUTS:
 * son func1 with int :0
 * father func1 with int parameter:1
 * father func1 with double parameter:2.3
 * son func1 with double:2.3
 * father func1 with string parameter:Hello
 * son func1 with string:Hello
 * *****************************
 * NOTE: son calls father's function
 *
 */
