#include <iostream>
using namespace std;
class HasPtrMem {
 public:
    HasPtrMem() : d(new int(0)) { }
    HasPtrMem(const HasPtrMem &h) : d(new int(*h.d)) { }
    ~HasPtrMem() { }

 private:
    int *d;
};

HasPtrMem GetTemp() { return HasPtrMem(); }

int main() {
    HasPtrMem a = GetTemp();
    HasPtrMem b = a;
}