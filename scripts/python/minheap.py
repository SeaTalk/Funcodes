#!/usr/bin/python
import string

class MinHeap(object):
    def __init__(self, cap):
        self.data=[]
        self.count=len(self.data)
        self.capacity=cap

    def size(self):
        return self.count

    def isEmpty(self):
        return self.count == 0

    def getData(self):
        return self.data

    def insert(self, item):
        if self.count < self.capacity:
            self.data.append(item)
            self.count += 1
            if self.count == self.capacity:
                return self.build_heap()
            return 0
        #self.shiftup(self.count)
        if item[1] > self.data[0][1] :
            self.data[0] = item
            return self.fix_heap(0)


    def build_heap(self):
        if self.count < 1:
            return -1
        middle = (self.count-1)/2
        while middle >= 0 :
            tmp_index = (middle << 1) + 1
            tmp_index2 = (middle + 1) << 1
            tmp = middle
            if tmp_index2 < self.count and  self.data[tmp_index][1] > self.data[tmp_index2][1]:
                tmp = tmp_index2
            else :
                tmp = tmp_index
            if tmp < self.count and self.data[tmp][1] < self.data[middle][1]:
                self.data[middle], self.data[tmp] = self.data[tmp], self.data[middle]
            middle = middle - 1
        return 0

    def fix_heap(self, ind):
        index = ind
        hasChild = True
        while index < self.count and hasChild == True:
            j = (index << 1) + 1
            k = (index + 1) << 1
            tmp = j
            hasChild = False
            if k < self.count and self.data[j][1] > self.data[k][1]:
                tmp = k
                hasChild = True
            else :
                tmp = j
                hasChild = True
            if tmp < self.count and self.data[tmp][1] < self.data[index][1]:
                self.data[index], self.data[tmp] = self.data[tmp], self.data[index]
                hasChild = True
            index = tmp
        return 0
