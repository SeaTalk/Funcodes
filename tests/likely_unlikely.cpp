#include <iostream>
using namespace std;

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int main() {
    if (likely(1 < 3)) {
        cout << "hello wrong..." << endl;
    }
    return 0;
}