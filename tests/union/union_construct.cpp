#include <iostream>
#include <string>
using namespace std;

/* 非受限联合体有非POD成员，而该非POD成员类型拥有非平凡的构造函数，
 * 那么非受限联合体成员的默认构造函数将被编译器删除。
 * 解决这个问题的方法是，程序员自己为非受限联合体定义构造函数。
 */

union T {
    string s;
    int n;
public:
    // 自定认构造函数和析构函数
    T() { new (&s) string; }
    T(string p) { new (&s) string(p); }
    T(int i) : n(i) {}
    // ~T() { s.~string(); }    // 必须保证T为string对象，否则析构出错
    ~T() {}
};

int main() {
    T t;
    T t1("hello");
    T t2(5);
    cout << t.s << endl;
    cout << t1.s << endl;
    cout << t2.n << endl;
}