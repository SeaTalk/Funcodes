#include <iostream>
#include <memory>

using namespace std;

struct Foo
{
    Foo() { cout << "Foo construct..." << endl; }
    ~Foo() { cout << "~Foo ..." << endl; }
};

int main() {
    unique_ptr<Foo> up(new Foo);
    up.reset();
    cout << (nullptr == up) << endl;
    return 0;
}
