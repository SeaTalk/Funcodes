#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class TestClass {
 public:
    TestClass() = delete;
    TestClass(int a) : id_(a) {}
    ~TestClass() {
        cout << "destruct TestClass id: " << id_ << endl;
    }

    bool operator==(const TestClass& t) {
        return t.id_ == id_;
    }

    void print() { cout << "id:" << id_ << endl; }

 private:
    int id_;

};

int main() {
    vector<TestClass> tv;
    tv.reserve(5);

    for (int i = 0; i < 5; ++i) {
        tv.emplace_back(i);
    }

    // be careful of the diff in performace
    auto it = std::find(tv.begin(), tv.end(), 3);
    cout << "it:"; it->print();
    cout << "=======emmmmm=========" << endl;
    it = std::find(tv.begin(), tv.end(), TestClass(3));
    cout << "it:"; it->print();

    return 0;
}