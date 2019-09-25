#include <iostream>
#include <vector>
using namespace std;

class A
{
public:
    A(){cout<<"constructor A"<<endl;}
    ~A(){cout<<"destructor A"<<endl;}
};

class B
{
public:
    B():a(),v(1,40){cout<<"constructor B"<<endl;}
    ~B(){cout<<"destructor B"<<endl;}
private:
    A a;
public:
    vector<int> v;
};

int main()
{
    B b;int i=0;
    for(vector<int>::iterator iter=b.v.begin();iter!=b.v.end();++iter,++i)
    {
        cout<<"v_"<<i<<":"<<*iter<<endl;
    }
}

/**
 * OUTPUT:
 * constructor A
 * constructor B
 * v_0:40
 * destructor B
 * destructor A
 **************************
 * As we can see, the destruction of B will cause 
 * the destruction of A,apparently! 
 * 
 * It is said that in some circumanstances, A can not be destroyed. 
 * Like discussed in https://blog.csdn.net/wu_nan_nan/article/details/46990315
 *
 * However, I run some tests in Folder "objectInit2", and find the opposite result.
 * Maybe the environment setting is the main cause. 
 *
 * I run the code in centos7 with gcc-4,8.5, and the author of that article using VS2010.
 */
