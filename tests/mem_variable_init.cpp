#include <stdlib.h>

#include <iostream>
using namespace std;

struct A {
    int a;
    float b;
    double c;
    const char* e;
};

std::ostream& operator<< (std::ostream &os, const A& a) {
    return os << a.a << ", " << a.b << ", " << a.c << ", " << a.e;
}

typedef struct {
int s;
double ss[];
} B;

int main() {
    const char HH[] = "Hello World!";
    A a {1,1.0, 3.3, HH};
    cout << a << endl;

    // B b;
    cout << sizeof(B) << endl;
    B *bv = (B*)malloc(sizeof(B) + sizeof(double) * 10);
    bv->s = 10;
    for (int i = 0; i < bv->s; ++i)
        bv->ss[i] = i * 2.1;

    for (int i = 0; i < bv->s; ++i)
        cout << bv->ss[i] << " ";
    cout << endl;

    free(bv);

    return 0;
}