#include <iostream>
#include <future>
#include <thread>

int get_value() {
    std::cout << "get value called" << std::endl;
    return 5;
}

void print_value() {
    std::cout << "print value: 5" << std::endl;
}

void do_other_stuff() {
    std::cout << "getting other stuff..." << std::endl;
}

int main() {
    std::future<int> value = std::async(std::ref(get_value));
    do_other_stuff();
    std::cout << "value is " << value.get() << std::endl;

    std::thread t(get_value);
    do_other_stuff();
    std::cout << "end" << std::endl;
    t.join();
    return 0;
}