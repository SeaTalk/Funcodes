#include <iostream>

using namespace std;

class F1 {
public:
    virtual void print() { cout << "f1 print" << endl; }
    virtual void print1() { cout << "f1 print1" << endl; }
};

class F2 {
public:
    virtual void print() { cout << "f2 print" << endl; }
    virtual void print1() { cout << "f2 print1" << endl; }
};

class son : public F1, public F2 {
public:
    void print() override {
        cout << "son print" << endl;
    }
};

int main() {
    F1 *a = new son;
    F2 *b = new son;
    a->print();
    b->print();
    a->print1();
    b->print1();
    delete a;
    cout << "a delete" << endl;
    delete b;
    cout << "b delete" << endl;
}