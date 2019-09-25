#ifndef CUSTOM_HEAP_H_
#define CUSTOM_HEAP_H_

#include <exception>
#include <memory>
#include <vector>
#include <functional>
#include <utility>

using ::std::vector;
using ::std::shared_ptr;
using ::std::function;
using ::std::exception;

struct wrong_capacity_heap : exception {
    const char* what() const throw() {
        return "heap capacity initialization error. a positive is expected";
    }
};

template<class T>
class CustomHeap {
public:
    CustomHeap(size_t cap, function<bool (T, T)> comp) :
            _capacity(std::move(cap)),
            _compare(std::move(comp)),
            _current_size(0) {

        if (0 >= _capacity) {
            throw wrong_capacity_heap();
        }
        _heap = std::shared_ptr<T>(new T[_capacity], deleter);
    }
    CustomHeap() = delete;
    CustomHeap(const CustomHeap& c) = delete;
    CustomHeap& operator= (const CustomHeap& c) = delete;

    int insert(T elem);
    bool heap_empty();
    int resize_heap();
    T get(int i);

private:
    int build_heap();
    int fix_heap();
    static void deleter(T *p);

    shared_ptr<T> _heap;
    size_t _capacity;
    size_t _current_size;
    function<bool (T, T)> _compare;
};

template <class T>
void CustomHeap<T>::deleter(T *p) {
    delete []p;
}

template <class T>
int CustomHeap<T>::insert(T item) {
    if (_current_size < _capacity) {
        _heap.get()[_current_size++] = item;
        if (_current_size == _capacity) {
            return build_heap();
        }
        return 0;
    }
    _heap.get()[0] = item;
    return fix_heap();
} 

template <class T>
int CustomHeap<T>::build_heap() {
    if (_current_size <= 0) {
        return -1;
    }
    int mid = (_current_size - 1) >> 1;
    while (mid >= 0) {
        int tmp_index = (mid << 1) + 1;
        int tmp_index2 = (mid + 1) << 1;
        int tmp = mid;
        if (tmp_index2 < _current_size and _compare(_heap.get()[tmp_index], _heap.get()[tmp_index2])) {
            tmp = tmp_index2;
        } else {
            tmp = tmp_index;
        }
        if (tmp < _current_size and _compare(_heap.get()[mid], _heap.get()[tmp])) {
            std::swap(_heap.get()[mid], _heap.get()[tmp]);
        }
        mid -= 1;
    }
    return 0;
}

template <class T>
int CustomHeap<T>::fix_heap() {
    int ind = 0;
    bool has_child = true;
    while (ind < _current_size and has_child) {
        int j = (ind << 1) + 1;
        int k = (ind + 1) << 1;
        int tmp = j;
        has_child = false;
        if (k < _current_size and _compare(_heap.get()[j], _heap.get()[ind])) {
            tmp = k;
            has_child = false;
        } else {
            tmp = j;
            has_child = true;
        }
        if (tmp < _current_size and _compare(_heap.get()[ind], _heap.get()[tmp])) {
            std::swap(_heap.get()[ind], _heap.get()[tmp]);
            has_child = true;
        }
        ind = tmp;
    }
    return 0;
}


template <class T>
bool CustomHeap<T>::heap_empty() {
    return _current_size <= 0;
}

template <class T>
int CustomHeap<T>::resize_heap() {
    if (_current_size <= 0) {
        return -1;
    }
    _heap.get()[0] = _heap.get()[--_current_size];
    if (_current_size > 0) {
        return build_heap();
    } else if (0 == _current_size) {
        return 1;
    } else {
        return -1;
    }
}


template <class T>
T CustomHeap<T>::get(int i) {
    if (i < 0 or i > _capacity) {
        return T();
    }
    return _heap.get()[i];
}

#endif
