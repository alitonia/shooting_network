#!/usr/bin/env python3
from queue import Queue
from threading import Thread
import socket
import logging
import threading
import time
import sys


def _start_send_thread(q: Queue):
    with socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) as s:
        while True:
            while q.empty() is False:
                data = q.get()
                address = data['IP']
                port = data['port']
                message = data['payload']
                s.sendto(message, (address, port))
            else:
                time.sleep(0.001)


def start_send_thread():
    q = Queue()
    try:
        x = threading.Thread(target=_start_send_thread, args=(q,))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    return q
