#include <utility>
#include <iostream>

class A {
 public:
    A() : counter(0) { std::cout << "normal constructor" << std::endl; }
    A(const A &o) : counter(1) {
        std::cout << "copy constructor" << std::endl;
    }
    A(A&& o) : counter(2) {
        std::cout << "right reference constructor" << std::endl;
    }
    A &operator==(const A &o) {
        std::cout << "copy assign" << std::endl;
        return *this;
    }
    A &operator==(A&& o) {
        std::cout << "right reference assign" << std::endl;
        return *this;
    }

    int counter;
};

void print_(int a, float b, A ac) {
    std::cout << "a:" << a << ", b:" << b << std::endl;
    std::cout << "ac:" << ac.counter << std::endl;
}

template<typename... Args>
void print_move(Args... args) {
    std::cout << "before move" << std::endl;
    print_(std::move(args)...);
}

template<typename... Args>
void print_forward(Args&&... args) {
    std::cout << "before forward" << std::endl;
    print_(std::forward<Args>(args)...);
}

int main() {
    print_move(10, 11.0f, A());
    std::cout << "<<<<<<<<<<<<<<<<<<<<" << std::endl;
    print_forward(23, 43.4f, A());
    return 0;
}

/* This example tests the differents between std::move and std::forward
 * The result is below
 * \\\\\\\\\\\\\\\\\\\\
 * normal constructor
 * before move
 * right reference constructor
 * a:10, b:11
 * ac:2
 * <<<<<<<<<<<<<<<<<<<<
 * normal constructor
 * before forward
 * right reference constructor
 * a:23, b:43.4
 * ac:2
 */
