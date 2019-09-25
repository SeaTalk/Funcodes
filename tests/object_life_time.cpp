#include <iostream>
using namespace std;

class A
{
public :
    A(){cout<<"A's default constructor"<<endl; b=0;}
    A(A *a){cout<<"A's another constructor"<<endl; b=a->b+1;}
    ~A(){cout<<"obj:"<<b<<"destruct"<<endl;}

public:
    int b;
};

int main()
{
    A a;
    if(2>1)
    {
        A b(&a);
        if (b.b < a.b)
        {
            return 0;
        }
    }
    {
        A c(&a);
        c.b = 3;
        cout << "c.b" << c.b<<endl;
        return 0;
    }
}
/****************************
 * OUTPUT:
 * A's default constructor
 * A's another constructor
 * obj:1destruct
 * A's another constructor
 * c.b3
 * obj:3destruct
 * obj:0destruct
 *************************** 
 * It seems like the objects in submodule is released earlyer. 
 * */
