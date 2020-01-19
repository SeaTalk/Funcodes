#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v {0, 1, 2, 3, 4};
    auto *p = v.data();
    cout << *(p + 3) << endl;
    return 0;
}