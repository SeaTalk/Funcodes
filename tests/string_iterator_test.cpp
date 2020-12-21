#include <unistd.h>
#include <functional>
#include <string>
#include <iostream>
using namespace std;

#define PRINT_IMPL(ok, a) do {          \
    if (ok) {                           \
        cout << "ok..., " << a << endl; \
    } else {                            \
        cout << "a: " << a << endl;     \
    }                                   \
} while (0)

template<bool is_ok, typename T>
void print(T a) {
    PRINT_IMPL(is_ok, a);
}

int main() {
    string s("12345");
    const char *i = s.begin().base();
    cout << "i: " << *i << endl;
    string ss;
    cout << "back: " << (int64_t)ss.end().base() << ", length: " << (int64_t)&ss[0] + ss.length() << std::endl;

    print<true>(s);
    print<false>(1);

    bool a = false;
    if (a = true) {
        print<true>(a);
    }

    int ret;
    // ret = access("/Users/tangjintao", F_OK);
    // cout << "ret: " << ret << endl;
    // ret = symlink("/Users/tangjintao/Data/tf_docs", "/Users/tangjintao/tf_docs");
    // cout << "ret: " << ret << endl;
    // ret = access("/Users/tangjintao/tf_docs", F_OK);
    // cout << "ret: " << ret << endl;
    // ret = remove("/Users/tangjintao/tf_docs");
    cout << "ret: " << ret << endl;


    auto f = std::bind(&print<true, int>, std::placeholders::_1);
    f(120);


    return 0;
}