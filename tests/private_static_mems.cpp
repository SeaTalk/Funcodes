#include <iostream>
using namespace std;

class A 
{
public:
    A(){++a;}
    A(A *h){h->a++;h->b++;}
    void print() const{ cout << "print static value:"<<a <<endl; }

    int add_and_get_a(){ return ++a; }

private:
    static int a ;
    int b ;

};

int A::a = 1;

int main()
{
    A a;
    cout << a.add_and_get_a() <<endl;
    A b(&a);
    b.print();
}
