#!/usr/bin/python
# -*- coding:utf-8 -*-

import os
import Queue
import threading
import contextlib

sequence=[]
lock = threading.RLock()

def PutIntoList(m):
    global lock
    global sequence
    sequence.append(m) # it seems like the list in python is threadsafe, no need to lock it

class ThreadPool(object):
    def __init__(self, max_num):
        self.StopEvent = 0  # 线程任务终止符，当线程从队列获取到StopEvent时，代表此线程可以销毁。可设置为任意与任务有区别的值。
        self.q = Queue.Queue()
        self.max_num = max_num  # 最大线程数
        self.terminal = False  # 是否设置线程池强制终止
        self.created_list = []  # 已创建线程的线程列表
        self.Deamon = False  # 线程是否是后台线程

    def run(self, func, args, callback=None):
        """
        线程池执行一个任务
        :param func: 任务函数
        :param args: 任务函数所需参数
        :param callback:
        :return: 如果线程池已经终止，则返回True否则None
        """
        if len(self.created_list) < self.max_num:
            self.create_thread()
        task = (func, args, callback,)
        self.q.put(task)

    def create_thread(self):
        """
        创建一个线程
        """
        t = threading.Thread(target=self.call)
        t.setDaemon(self.Deamon)
        t.start()
        self.created_list.append(t)  # 将当前线程加入已创建线程列表created_list

    def call(self):
        """
        循环去获取任务函数并执行任务函数
        """
        current_thread = threading.current_thread()  # 获取当前线程对象·
        event = self.q.get()  # 从任务队列获取任务
        while event != self.StopEvent:  # 判断获取到的任务是否是终止符
            func, arguments, callback = event  # 从任务中获取函数名、参数、和回调函数名
            try:
                result = func(*arguments)
                func_excute_status = True  # func执行成功状态
            except Exception as e:
                func_excute_status = False
                result = None
                print('函数执行产生错误', e)  # 打印错误信息)
            if func_excute_status:  # func执行成功后才能执行回调函数
                if callback is not None:  # 判断回调函数是否是空的
                    try:
                        callback(result)
                    except Exception as e:
                        print('回调函数执行产生错误', e)  # 打印错误信息)
            if self.terminal:  # 判断线程池终止命令，如果需要终止，则使下次取到的任务为StopEvent。
                event = self.StopEvent
            else:  # 否则继续获取任务
                event = self.q.get()  # 当线程等待任务时，q.get()方法阻塞住线程，使其持续等待
        else:  # 若线程取到的任务是终止符，就销毁线程
            # 将当前线程从已创建线程列表created_list移除
            self.created_list.remove(current_thread)

    def close(self):
        """
        执行完所有的任务后，所有线程停止
        """
        full_size = len(self.created_list)  # 按已创建的线程数量往线程队列加入终止符。
        while full_size:
            self.q.put(self.StopEvent)
            full_size -= 1

    def terminate(self):
        """
        无论是否还有任务，终止线程
        """
        self.terminal = True
        while self.created_list:
            self.q.put(self.StopEvent)
        self.q.queue.clear()  # 清空任务队列

    def join(self):
        """
        阻塞线程池上下文，使所有线程执行完后才能继续
        """
        for t in self.created_list:
            t.join()

pool=ThreadPool(5)
for i in range(20000):
    pool.run(PutIntoList,(str(i)+'\n',))

pool.close()
pool.join()

with open("../../bin/test.txt","w") as f:
    f.writelines(sequence)
