from queue import Queue
from threading import Thread
import socket
import logging
import threading
import time
import sys
import pygame

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 8897  # Port to listen on (non-privileged ports are > 1023)


def _start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    while True:
        while recv_q.empty() is False :
            msg = recv_q.get()
            config['msg'].append(msg)
        time.sleep(0.001)


def start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    try:
        x = threading.Thread(target=_start_process_thread, args=(recv_q, send_q, config))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    # return q
