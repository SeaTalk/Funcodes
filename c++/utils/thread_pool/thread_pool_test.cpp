#include <iostream>
using namespace std;

#include "thread_pool.h"

int main () {
    ThreadPool pool;
    for (int i = 0; i < 10; ++i) {
        pool.submit([=](){std::cout << "i" << i << std::endl;});
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return 0;
}