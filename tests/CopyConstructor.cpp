#include <iostream>
using namespace std;

class A
{
private:
    int somevalue;
public:
    A():somevalue(11){cout<<"this is A's default constructor"<<endl;}
    A(const int &value):somevalue(value){cout<<"this is A's normal constructor"<<endl;}
    A(const A &a){cout<<"this is A's copy constructor"<<endl;this->somevalue = a.somevalue + 1000;}
    void operator =(const A &a){cout<<"this is A's copy operation"<<endl;this->somevalue=13;}
    void print(){cout<<this->somevalue<<endl;}
};

A DoSomething()
{
    A a;
    return a;
}

A DoSth()
{
    A a(12);
    return a;
}

int main()
{
    A a = DoSomething(); // initialization
    a.print();
    a = DoSth(); // assignment
    a.print();
    return 0;
}

/***********************************
 *
 ************ OUTPUTS:**************
 *
 * this is A's default constructor
 * this is A's copy constructor
 * this is A's copy constructor
 * 2011
 * this is A's normal constructor
 * this is A's copy constructor
 * this is A's copy operation
 * 13
 ***********************************
 * This is an interesting test,and we get an interesting result.
 * In this test, we can see the differences of initialization and assignment, though both of them using operator"=".
 * 
 * NOTATION:
 * In this test, I used opt "-fno-elide-constructors" when compiling this file by using command line below:
 *    g++ -o ../bin/copy_construct CopyConstructor.cpp -fno-elide-constructors
 * It seems like, without "-fno-elide-constructors", the g++ compilor will automatically optimize the process of initialization and assignment, and the result will be very different.
 * Its result is like below:
 *
 * this is A's default constructor
 * 11
 * this is A's normal constructor
 * this is A's copy operation
 * 13
 *
 * As we can see, g++ compiler reduce the object copy construct process which makes the program more effective!!!
 *
 *
 */
