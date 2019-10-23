#include <iostream>

class A {
public:
    void echo(int i) const { std::cout << "echoing : " << i << std::endl;}
};

const A& getA() {
    A a;
    return a;
}

int main() {
    const A& a = getA();
    a.echo(5);
    return 0;
}