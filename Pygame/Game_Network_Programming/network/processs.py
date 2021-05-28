from queue import Queue
from threading import Thread
import socket
import logging
import threading
import time
import sys
import pygame
import datetime
import re
import time
import json

HOST = '127.0.0.1'
PORT = 8897

C_PORT = 8996

cooldown = {
    'register': {
        'time': 0.5,
        'last': None
    },
    'get_room': {
        'time': 0.5,
        'last': None
    }
}


def get_permission(key):
    if cooldown[key]['last'] is None:
        cooldown[key]['last'] = time.time()
        return True
    elif time.time() - cooldown[key]['last'] > cooldown[key]['time']:
        cooldown[key]['last'] = time.time()
        return True
    else:
        return False


def _start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    while True:
        try:
            sid = config['self_id']

            while recv_q.empty() is False:
                should_forward = True
                msg = recv_q.get()

                # id
                if re.match(r"^register_id (\d+)$", msg) is not None and config['self_id'] is None:
                    print(re.match(r"^register_id (\d+)$", msg).groups()[0])
                    config['self_id'] = int(re.match(r"^register_id (\d+)$", msg).groups()[0])
                    should_forward = False

                # players
                elif re.match(r"^register_details (.*)$", msg) is not None and config['other_players'] is None:
                    jstr = re.match(r"^register_details (.*)$", msg.rstrip()).groups()[0]
                    room_des = json.loads(jstr)
                    if room_des.get('players'):
                        config['other_players'] = [int(i) for i in room_des['players'] if int(i) != config['self_id']]
                        print(config['other_players'], 'yes')
                    should_forward = False

                if should_forward:
                    config['msg'].append(msg)

            while len(config['event']) != 0:
                should_send = True
                msg = config['event'].pop()

                # register = re.search(r"^register_id (\d+)", msg)
                register = re.search(r"^register.?$", msg)
                get_room = re.search(r"^get_room.?$", msg)

                if register is not None:
                    should_send = get_permission('register')
                elif get_room is not None:
                    should_send = get_permission('get_room')

                # print(f"{sid} {msg}".encode('utf-8'))

                if should_send:
                    send_q.put({
                        "IP": HOST,
                        "port": C_PORT,
                        "payload": f"{sid} {msg}--py".encode('utf-8')
                    })

            time.sleep(0.001)
        except Exception as e:
            print(e)


def start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    try:
        x = threading.Thread(target=_start_process_thread, args=(recv_q, send_q, config))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    # return q
