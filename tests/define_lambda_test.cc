#include <iostream>
#include <string>
using namespace std;

#define FIND [&](char ch) { \
    cout << " " << ch << b; \
}

int main() {
    string a("la;sdfj;aldksf");
    const char b = '\n';
    for_each(a.begin(), a.end(), FIND);
    return 0;
}