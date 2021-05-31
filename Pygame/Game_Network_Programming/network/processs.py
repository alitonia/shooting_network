from queue import Queue
import threading
import sys
import re
import time
import json

cooldown = {
    'register': {
        'time': 1,
        'last': None
    },
    'get_room': {
        'time': 1,
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

            # Receive message
            while recv_q.empty() is False:
                should_forward = True
                msg = recv_q.get()
                if re.match(r"^register_id (\d+)$", msg) is not None and config['self_id'] is None:
                    print(re.match(r"^register_id (\d+)$", msg).groups()[0])
                    config['self_id'] = int(re.match(r"^register_id (\d+)$", msg).groups()[0])
                    should_forward = False

                # players
                elif re.match(r"^register_details (.*)$", msg) is not None:
                    if config['other_players'] is None:
                        jstr = re.match(r"^register_details (.*)$", msg.rstrip()).groups()[0]
                        room_des = json.loads(jstr)
                        if room_des.get('player_list'):
                            print('-----')
                            print(room_des)
                            config['other_players'] = [int(i) for i in room_des.get('player_list') if
                                                       int(i) != config['self_id']]
                            print(config['other_players'], 'yes')

                            print('-----')
                    should_forward = False

                if should_forward:
                    config['msg'].append(msg)

            # Send messages
            while len(config['event']) != 0:
                should_send = True
                msg = config['event'].pop()
                # register = re.search(r"^register_id (\d+)", msg)

                register = re.search(r"^register.?$", msg)
                get_room = re.search(r"^get_room.?$", msg)

                if register is not None:
                    should_send = get_permission('register')
                    if should_send:
                        send_q.put({
                            "IP": config['HOSTIP'],
                            "port": config['C_PORT'],
                            "payload": f"{msg}--py".encode('utf-8')
                        })
                    continue

                elif get_room is not None:
                    should_send = get_permission('get_room')
                    msg = 'join_room'

                # print(f"{sid} {msg}".encode('utf-8'))

                if should_send:
                    send_q.put({
                        "IP": config['HOSTIP'],
                        "port": config['C_PORT'],
                        "payload": f"{sid} {msg}--py".encode('utf-8')
                    })

            time.sleep(0.001)
        except Exception as e:
            print(e)
            print('In process thread')


def start_process_thread(recv_q: Queue, send_q: Queue, config: dict):
    try:
        x = threading.Thread(target=_start_process_thread, args=(recv_q, send_q, config))
        x.start()
    except e:
        sys.exit("Error: unable to start thread")
    # return q
