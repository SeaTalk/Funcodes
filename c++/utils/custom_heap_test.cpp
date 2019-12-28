//#include "custom_max_heap.hpp"
#include "custom_heap.hpp"
#include <iostream>

using namespace std;

class data_helper {
public:
    data_helper(int i, int v):
        index(i), value(v) {}
    data_helper():data_helper(-1, -1) {}
    data_helper(const data_helper& data) {
        index = data.index;
        value = data.value;
    }
    data_helper(data_helper* data) {
        if (data == nullptr) {
            index = -1;
            value = -1;
        } else {
            index = data->index;
            value = data->value;
        }
    }
    ~data_helper() {}
    data_helper& operator= (const data_helper& data) {
        index = data.index;
        value = data.value;
        return *this;
    }
    int index;
    int value;
};

bool compare(const data_helper& data1, const data_helper& data2) {
    return data1.value > data2.value;
}

int main() {
    vector<data_helper>* heap_space = new vector<data_helper>();
    int data[][4] = {
        {1,2,3,15}, // every array is increamental. [VERY IMPORTANT]
        {4,5,6,9},
        {7,8,9,10}
    };
    function<bool (const data_helper&, const data_helper&)> fun(compare);
    // CustomMaxHeap<data_helper> heap(3, shared_ptr<vector<data_helper>>(heap_space), fun);
    CustomHeap<data_helper> heap2(3, fun);
    int cur_index[3] = {0};
    int cur_index2[3] = {0};
    for (int i = 0; i < 3; ++i) {
        // heap.insert(data_helper(i,data[i][cur_index[i]]), nullptr);
        heap2.insert(std::move(data_helper(i, data[i][cur_index2[i]])));
    }
/*
    while (!heap.heap_empty()) {
        auto tmp = (*heap_space)[0];
        cout << "value:" << (tmp).value << " , index:" << (tmp).index << endl;
        ++ cur_index[(tmp).index];
        if (cur_index[(tmp).index] >= 4) {
            heap.resize_heap(nullptr);
        } else {
            heap.insert(data_helper((tmp).index,data[(tmp).index][cur_index[(tmp).index]]), nullptr);
        }
    }
*/
    while (!heap2.heap_empty()) {
        auto tmp = heap2.get(0);
        cout << "value:" << tmp.value << " , index:" << tmp.index << endl;
        ++ cur_index2[tmp.index];
        if (cur_index2[tmp.index] >= 4) {
            heap2.resize_heap();
        } else {
            heap2.insert(data_helper(tmp.index,data[tmp.index][cur_index2[tmp.index]]));
        }
    }
    return 0;
}
