from queue import Queue
from threading import Thread
import socket
import logging
import threading
import time
import sys
import pygame

HOST = '127.0.0.1'
PORT = 8897

C_PORT = 8996


def _start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    while True:
        sid = config['self_id']
        while recv_q.empty() is False:
            msg = recv_q.get()
            config['msg'].append(msg)
        while len(config['event']) != 0:
            msg = config['event'].pop()
            print(f"{sid} {msg}".encode('utf-8'))
            send_q.put({
                "IP": HOST,
                "port": C_PORT,
                "payload": f"{sid} {msg}--py".encode('utf-8')
            })

        time.sleep(0.001)


def start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    try:
        x = threading.Thread(target=_start_process_thread, args=(recv_q, send_q, config))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    # return q
