#!/bin/python
# -*- coding:utf-8 -*-
import os, sys
from ctypes import c_char_p
from multiprocessing import Process, Queue, Lock, Value
from framework.message.remote import NineNClient
from threading import Thread


class ThreadPool(object):
    StopEvent = -1
    GetRequestEvent = 1
    PredictEvent = 2
    NormalEvent = 3
    DumpRequestEvent = 4

    def __init__(self, max_num, model_name, model_version, input_type,
                 shared_feature_dict, host, port, output_dir):
        self.q = Queue()
        self.process_q = Queue()
        self.max_num = max_num
        self.terminal = False
        self.created_threads = Value('l', 0)
        self.created_process = []
        self.Deamon = False
        self.lock = Lock()
        self.model_name = Value(c_char_p, model_name.encode('utf-8'))
        self.model_version = Value('l', model_version)
        self.input_type = Value(c_char_p, input_type.encode('utf-8'))
        self.feature_dict = shared_feature_dict
        self.host = Value(c_char_p, host.encode('utf-8'))
        self.port = Value('l', port)
        self.output_dir = Value(c_char_p, output_dir.encode('utf-8'))

    def run(self, event_type, func, args, callback=None):
        if self.created_threads.value < self.max_num:
            with self.lock:
                self.create_thread()
        task = (
            event_type,
            func,
            args,
            callback,
        )
        self.q.put(task)

    def create_thread(self):
        t = Process(target=self.call)
        t.start()
        self.created_threads.value += 1
        self.created_process.append(t)

    def call(self):
        current_thread = os.getpid()
        # gen 9n client
        client = NineNClient(model_name=self.model_name.value.decode("utf-8"),
                             model_version=self.model_version.value,
                             output_str="NULL",
                             input_type=self.input_type.value.decode('utf-8'),
                             feature_dict=self.feature_dict,
                             output_tf_type="NULL",
                             host=self.host.value.decode('utf-8'),
                             port=self.port.value,
                             with_device=True)
        output_dir = self.output_dir.value.decode('utf-8')
        req_list = []
        index = -1
        event = self.q.get()
        while event != ThreadPool.StopEvent:
            event_type, func, arguments, callback = event
            try:
                if event_type == ThreadPool.PredictEvent:
                    if len(req_list) > 0:
                        index += 1
                        index = index % len(req_list)
                        func(client, req_list[index], *arguments)
                else:
                    result = func(*arguments)
                func_excute_status = True
            except Exception as e:
                func_excute_status = False
                result = None
                print('func execute failed', e)
            if func_excute_status:
                if callback is not None:
                    try:
                        if event_type == ThreadPool.GetRequestEvent:
                            callback(client, result, req_list)
                        elif event_type == ThreadPool.DumpRequestEvent:
                            callback(client, result, output_dir, arguments[1])
                        else:
                            callback(result)
                    except Exception as e:
                        print('recall failed', e)
            if self.terminal:
                event = ThreadPool.StopEvent
            else:
                event = self.q.get()
        else:
            with self.lock:
                self.created_threads.value -= 1

    def close(self):
        full_size = self.created_threads.value
        while full_size:
            self.q.put(ThreadPool.StopEvent)
            full_size -= 1

    def join(self):
        for t in self.created_process:
            t.join()

