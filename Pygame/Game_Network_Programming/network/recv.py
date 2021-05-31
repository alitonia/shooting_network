#!/usr/bin/env python3
from queue import Queue
import socket
import threading
import sys

def _start_receive_thread(q, cf):
    with socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) as s:
        s.bind((cf['HOSTIP'], cf['PY_PORT']))
        while True:
            data = s.recvfrom(1024)
            q.put(data[0].decode('utf-8'))


def start_receive_thread(cf):
    q = Queue()
    try:
        x = threading.Thread(target=_start_receive_thread, args=(q, cf))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    return q
