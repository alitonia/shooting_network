var PORT = 8998;

const {nanoid} = require('nanoid')

var dgram = require('dgram');
var server = dgram.createSocket('udp4');

// const createId = nanoid
const inmemData = {
    rooms: {
        'li5SdBU-ff33qW0LhwV8q': {
            player_startpoint: 2,
            player_list: [1, 2, 3],
            status: 'queuing'
        },
        'li5SdBU-ff33qW0LhwV8e': {
            player_startpoint: 2,
            player_list: [4],
            status: 'queuing'
        },
        'li5SdBU-ff33qW0LhwV8r': {
            player_startpoint: 2,
            player_list: [48, 9],
            status: 'playing'
        }
    },
    playerIds: [1, 2, 3, 4, 48, 9],
    idMapIp: {
        1: '127.0.0.1',
        2: '127.0.0.1',
        3: '127.0.0.1',
        4: '127.0.0.1',
        48: '127.0.0.1',
        9: '127.0.0.1',
    }
}

const rstrip = (someText) => someText.replace(/(\r\n|\n|\r)/gm, "")

const parseMessage = (msg) => {
    return {
        payload: rstrip(msg)
    }
}

const connector_id = (address, port) => {
    return address + '_' + port
}

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {
        const {address, port, size} = remote
        console.log('received ' + address + ':' + port + '_' + size + ' ' + ' - ' + message)

        const {payload} = parseMessage(message.toString())
        console.log(payload)

        if (/.*create_room \d+.*/i.test(payload)) {
            const playerId = Number.parseInt(payload.split(' ')[1])
            inmemData.idMapIp[playerId] = address

            if (!inmemData.playerIds.includes(playerId)) {

                const roomId = nanoid();
                const room = {
                    player_startpoint: 2,
                    player_list: [playerId]
                } // pending room

                inmemData.rooms[roomId] = room
                inmemData.playerIds.push(playerId)


                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/.*join_room \d+.*/i.test(payload)) {
            const playerId = Number.parseInt(payload.split(' ')[1])
            inmemData.idMapIp[playerId] = address
            if (inmemData.playerIds.includes(playerId)) {

                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                let room = Object.values(inmemData.rooms).find(x => x.player_list.length < x.player_startpoint && x.status !== 'playing')
                if (room) {
                    room.player_list.push(playerId)
                    inmemData.playerIds.push(playerId)
                } else {
                    const roomId = nanoid();
                    room = {
                        player_startpoint: 2,
                        player_list: [playerId]
                    } // pending room

                    inmemData.rooms[roomId] = room
                    inmemData.playerIds.push(playerId)
                }
                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/.*list_room.*/i.test(payload)) {
            const msg = JSON.stringify(inmemData.rooms)
            server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
        } else if (/.*delete_id \d+.*/i.test(payload)) {
            const playerId = Number.parseInt(payload.split(' ')[1])
            if (inmemData.playerIds.includes(playerId)) {
                delete inmemData.idMapIp[playerId]

                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                if (room.player_list.length === 1) {
                    const roomKey = Object.keys(inmemData.rooms).find(x => inmemData.rooms[x].player_list.includes(playerId))
                    delete inmemData.rooms[roomKey]
                } else {
                    room.player_list = room.player_list.filter(x => x !== playerId)
                }
                inmemData.playerIds = inmemData.playerIds.filter(x => x !== playerId)
                const payload = {
                    status: 1
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                const payload = {
                    status: 0
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/.*start_id \d+.*/i.test(payload)) {
            // dispatch all
            const playerId = Number.parseInt(payload.split(' ')[1])
            inmemData.idMapIp[playerId] = address

            if (inmemData.playerIds.includes(playerId)) {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                if (room.player_list.length < room.player_startpoint && room.status !== 'playing') {
                    // few people
                    const payload = {
                        status: 2,
                        message: "Not enough people"
                    }
                    const msg = JSON.stringify(payload)
                    server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
                } else {
                    room.status = 'playing'
                    const IPs = room.player_list.filter(x => x !== playerId).map(k => inmemData.idMapIp[k]).filter(x => x !== undefined)
                    const payload = {...room, IPs: IPs}
                    const msg = JSON.stringify(payload)
                    server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
                }
            } else {
                const payload = {
                    status: 0,
                    message: "Not found id"
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/.*delete_room \d+.*/i.test(payload)) {
            const playerId = Number.parseInt(payload.split(' ')[1])
            if (inmemData.playerIds.includes(playerId)) {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                inmemData.playerIds = inmemData.playerIds.filter(x => !room.player_list.includes(x))
                room.player_list.forEach(k => inmemData.idMapIp[k] = undefined)

                const roomKey = Object.keys(inmemData.rooms).find(x => inmemData.rooms[x].player_list.includes(playerId))
                inmemData.rooms[roomKey] = undefined

                const payload = {
                    status: 0,
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                const payload = {
                    status: 0,
                    message: "Not found id"
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }

        }
    }
);

server.bind(PORT);
