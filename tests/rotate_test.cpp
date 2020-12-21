#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

template<typename T>
struct Print{
  void operator() (const T &a) {
      std::cout << a << std::endl;
      d << a << '\n';
  }
};

int main() {
    vector<int> v = {1 ,2 ,3, 4 , 5};

    std::rotate(v.begin(), v.begin() + 2, v.end());

    for_each(v.begin(), v.end(), Print<int>());

    std::int8_t b = 43;
    char a = b;
    std::cout << "a:" << a << std::endl;

    return 0;
}