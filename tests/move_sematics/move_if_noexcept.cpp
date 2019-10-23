#include <iostream>
#include <utility>
using namespace std;

struct Maythrow {
    Maythrow() {}
    Maythrow(const Maythrow &) {
        std::cout << "Maythrow copy constructor." << endl;
    }
    Maythrow(Maythrow&&) {
        std::cout << "Maythrow move constructor." << endl;
    }
};

struct Nothrow {
    Nothrow() {}
    Nothrow(Nothrow&&) noexcept {
        std::cout << "Nothrow move constructor." << endl;
    }
    Nothrow(const Nothrow&) {
        std::cout << "Nothrow copy constructor." << endl;
    }
};

int main() {
    Maythrow m;
    Nothrow n;
    Maythrow mt = move_if_noexcept(m);  // Maythrow copy constructor
    Nothrow nt = move_if_noexcept(n);   // Nothrow move constructor
    return 0;
}