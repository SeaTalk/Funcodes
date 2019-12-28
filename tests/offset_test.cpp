#include <iostream>
using namespace std;

struct A {
    char a;
    int b;
    short c;
    double e;
};
struct B {
    char a;
    short b;
    int c;
    float e;
};

int main() {
    int array[2019] = { 0 };
    array[19] = 2019;
    std::cout << (unsigned long)array << std::endl;
    auto a = (unsigned long)((short *)array + 2019);
    std::cout << a << std::endl;
    auto c = (unsigned long)*(unsigned char *)(array + 19);
    std::cout << c << std::endl;
    auto b = (unsigned long)(array + *(unsigned char *)(array + 19));
    std::cout << b << std::endl;
    unsigned long offset = a - b;
    std::cout << offset << std::endl;

    std::cout << sizeof(struct A) << "  " << sizeof(struct B) << std::endl;
    A aa = {'c', 10, 11, 2.0};
    B bb = {'b', 11, 10, 1.0f};

    std::cout << *(short*)((char*)(&aa) + 8) << std::endl;
    std::cout << *(int*)((char*)(&bb) + 4) << std::endl;
    return 0;
}
