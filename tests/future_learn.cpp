#include <iostream>
#include <future>

int get_value() {
    std::cout << "get value called" << std::endl;
    return 5;
}

void do_other_stuff() {
    std::cout << "getting other stuff..." << std::endl;
}

int main() {
    std::future<int> value = std::async(get_value);
    do_other_stuff();
    std::cout << "value is " << value.get() << std::endl;
    return 0;
}