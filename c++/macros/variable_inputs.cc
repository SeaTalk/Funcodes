#include <iostream>

void print(int a) { std::cout << "a: " << a << std::endl; }
void print(int a, float b) { std::cout << "Two args func" << std::endl; }

#define PRINT_ONE(value) print(value)
#define PRINT_TWO(v1, v2) print(v1, v2)

#define VALIST(arg0, arg1, arg2, ...) arg2

#define ChooseProperFunc(...) VALIST(__VA_ARGS__, PRINT_TWO, PRINT_ONE)(__VA_ARGS__)

#define PRINT(...) ChooseProperFunc(__VA_ARGS__)

int main() {
    PRINT(1);
    PRINT(2, 3);
    return 0;
}
