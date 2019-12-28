#include <iostream>
using namespace std;

class X {};

class Y : public virtual X {};

class Z : public virtual X {};

class A : public Y, public Z {};

struct B {
    int val;
    //char c;
    //char d;
};
struct C : public B {
    char f;
};

int main() {
    cout << "size of X:" << sizeof(X) << endl;
    cout << "size of Y:" << sizeof(Y) << endl;
    cout << "size of Z:" << sizeof(Z) << endl;
    cout << "size of A:" << sizeof(A) << endl;
    cout << "size of B:" << sizeof(B) << endl;
    cout << "size of C:" << sizeof(C) << endl;
    return 0;
}
