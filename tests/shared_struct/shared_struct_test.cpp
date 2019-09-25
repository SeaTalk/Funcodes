#include <iostream>
#include <memory>

using namespace std;

struct ATest{

    ATest(int i):value(i) {}
    ATest() : ATest(-1) {}
    ATest(const ATest& t) {
        value = t.value;
    }
    ATest(const ATest* t) {
        value = t->value;
    }

    int value;
};

template<typename T>
class Container{
public:
    Container(int i) : _size(i), _current_index(0) {
        if (i > 0)
            _array = shared_ptr<T>(new T[i]);
        else
            _array = nullptr;
    }

    shared_ptr<T> get(int i) {
        if (i < 0 or i > _size) {
            return nullptr;
        }
        return shared_ptr<T>(new T(_array.get()[i]));
    }

    int insert(T t) {
        _array.get()[_current_index++] = std::move(t);
        if (_current_index == _size) {
            _current_index = 0;
        }
    }
private:
    int _size;
    int _current_index;
    shared_ptr<T> _array;
};

int main() {
    Container<ATest> my_container(3);
    for (int i = 0; i < 5; ++i) {
        my_container.insert(ATest(i));
    }
    for (int i = 0; i < 3; ++i) {
        auto item = my_container.get(i);
        cout << "value: " << item->value << endl;
    }
    return 0;
}
