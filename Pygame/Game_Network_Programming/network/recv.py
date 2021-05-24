#!/usr/bin/env python3
from queue import Queue
from threading import Thread
import socket
import logging
import threading
import time
import sys
import pygame

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 8997  # Port to listen on (non-privileged ports are > 1023)


def _start_receive_thread(q):
    with socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) as s:
        s.bind((HOST, PORT))
        while True:
            data = s.recvfrom(1024)
            q.put(data)
            print(data)


def start_receive_thread():
    q = Queue()
    try:
        x = threading.Thread(target=_start_receive_thread, args=(q,))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    return q
