#!/usr/bin/python

import os
import sys
import getopt
import string

def CommandExec(cmd):
    """
    """
    cmd_res_str = os.popen(cmd).read()
    return cmd_res_str#.find("ZOO_ERRORasdfasdf")
msg = CommandExec('./zkcli.sh -server 127.0.0.1:2181create /aba 2>&1')

# we can use os.popen(cmd).readlines() here to get lines of result
for m in msg.split('\n'):
    if m.strip() != '' :
        print "new line:", m.strip()


array=[]
def getArray():
    global array
    path="/"; print path[0], len(path)
    array=path[1:].split('/')

getArray()
print len(array)
print len(array)
print 'aaa'
for s in array:
    print s
print 'bbb'


gpus = sys.argv
for g in gpus:
    print g

print gpus[0]

argv = sys.argv
opts,args=getopt.getopt(argv[1:], "hc:r",["help","config","run"])

for opt, arg in opts:
    print opt, arg
