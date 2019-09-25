#ifndef _CUSTOM_MAX_HEAP_H
#define _CUSTOM_MAX_HEAP_H

#include <memory>
#include <vector>
#include <functional>
#include <exception>

using ::std::vector;
using ::std::shared_ptr;
using ::std::function;
using ::std::exception;

struct wrong_capacity_heap_exception : public exception {
    const char* what() const throw() {
        return "heap initialization error. A positive is required for heap capacity.";
    }
};

template <typename T>
class CustomMaxHeap {
public:
    CustomMaxHeap(int cap, shared_ptr<vector<T>> heap, function<bool (const T&, const T&)> comp) :
        _capacity(std::move(cap)), _index(0), _compare(std::move(comp)), _heap(std::move(heap)) {
        if (0 >= _capacity) {
            throw wrong_capacity_heap_exception();
        }
   }

    CustomMaxHeap() = delete;
    CustomMaxHeap(const CustomMaxHeap&) = delete;
    CustomMaxHeap& operator= (const CustomMaxHeap&) = delete;

    int insert(T element, T* out) {
        if (_index < _capacity) {
            _heap->push_back(element);
            ++_index;
            if (_index == _capacity) {
               return build_heap(out);
            }
            return 1;
        }
        (*_heap)[0] = std::move(element);
        return fix_heap(out);
    }

    int build_heap(T* out) {
        if (_capacity < 1) {
            return -1;
        }
        int mid = (_capacity - 1) >> 1;
        while (mid >= 0) {
            int tmp_index1 = (mid << 1) + 1;
            int tmp_index2 = (mid + 1) << 1;
            int tmp = mid;
            if (tmp_index2 < _capacity &&
                _compare(*(_heap->begin() + tmp_index1), *(_heap->begin() + tmp_index2))) {
                tmp = tmp_index2;
            } else {
                tmp = tmp_index1;
            }
            if (tmp < _capacity &&
                _compare(*(_heap->begin() + mid), *(_heap->begin() + tmp))) {
                T tmp_ptr = *(_heap->begin() + tmp);
                *(_heap->begin() + tmp) = *(_heap->begin() + mid);
                *(_heap->begin() + mid) = tmp_ptr;
            }
            mid = mid - 1;
        }
        if (nullptr != out) {
            *out = (*(_heap))[0];
        }
        return 0;
    }

    int fix_heap(T* out) {
        int index = 0;
        bool has_child = true;
        while (index < _capacity and has_child) {
            int j = (index << 1) + 1;
            int k = (index + 1) << 1;
            int tmp = j;
            has_child = false;
            if (k < _capacity and
                _compare(*(_heap->begin() + j), *(_heap->begin() + index))) {
                tmp = k;
                has_child = false;
            } else {
                tmp = j;
                has_child = true;
            }
            if (tmp < _capacity and
                _compare(*(_heap->begin() + index), *(_heap->begin() + tmp))) {
                T tmp_ptr = *(_heap->begin() + tmp);
                *(_heap->begin() + tmp) = *(_heap->begin() + index);
                *(_heap->begin() + index) = tmp_ptr;
            }
            index = tmp;
        }
        if (nullptr != out) {
            *out = (*(_heap))[0];
        }
        return 0;
    }

    int resize_heap(T* out) {
        auto iter = _heap->begin();
        if (_heap->end() == iter) {
            return -1;
        }
        (*_heap)[0] = (*_heap)[_capacity - 1];
        -- _capacity;
        if (_capacity > 0)
            return build_heap(out);
        else if (0 == _capacity) {
            if (nullptr != out) {
                *out = (*(_heap))[0];
            }
            return 1;
        } else {
            return -1;
        }
    }

    bool heap_empty() {
        return _capacity <= 0;
    }

private:
    int _capacity;
    int _index;
    function<bool(const T&, const T&)> _compare;
    shared_ptr<vector<T>> _heap;
};

#endif

