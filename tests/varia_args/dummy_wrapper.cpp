#include <iostream>
using namespace std;

template<typename ... T>
void DummyWrapper(T ...t) {}

template<typename T>
T pr(T t) {
    cout << t;
    return t;
}

template<typename... A>
void VTPrint(A... a) {
    DummyWrapper(pr(a)...);
}

int main() {
    VTPrint(1, ", ", 1.2, ". abc\n");
}