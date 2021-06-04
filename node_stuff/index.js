require('dotenv').config({path: './../.env'})

const PORT = process.env.NODE_PORT;

const shouldForward = (process.env.P2P ==1)

const shouldAddDummy = (process.env.HAS_DUMMY_ROOM ==1)
const DUMMY_ID = Number.parseInt(process.env.DUMMY_ID)
const DUMMY_SOC = process.env.DUMMY_SOC



const {nanoid} = require('nanoid')
const {md5} = require('md5')

var dgram = require('dgram');
var server = dgram.createSocket('udp4');

const DEFAULT_PLAYER_STARTPOINT = (process.env.DEFAULT_ROOM_SIZE || 2)

function getCurrentSec(){
  return Date.now()/1000
}


// const createId = nanoid
const inmemData = {
    rooms: {
        // note: 😘 disable this at playtime
        'li5SdBU-ff33qW0LhwV8q': {
            player_startpoint: DEFAULT_PLAYER_STARTPOINT,
            player_list: [1, 2, 3],
            status: 'queuing',
            lastModified: getCurrentSec()
        },
    },
    playerIds: [1, 2, 3],
    idMapIp: {
        1: '127.0.0.1:8996',
        2: '127.0.0.1:9998',
        3: '127.0.0.1:9992',
    }
}


if (shouldAddDummy) {
    const fillID = DUMMY_ID;
    const fillSOC = DUMMY_SOC;

    inmemData.rooms['li5SdBU-ff33qW0LhwV8e'] = {
        player_startpoint: DEFAULT_PLAYER_STARTPOINT,
        player_list: [fillID],
        status: 'queuing',
        lastModified: getCurrentSec()
    }

    inmemData.playerIds= inmemData.playerIds.concat(DUMMY_ID)
    inmemData.idMapIp[DUMMY_ID] = fillSOC
}


const MAX_PLAYER = 1000

const clearRoom =()=> {
    console.log('runnint')
    const runningTime = getCurrentSec();
    const removable = [];

    Object.keys(inmemData).forEach((k)=>{
        const room = inmemData[k]
        if (room) {
            const lm = room.lastModified;
            if (lm && runningTime - lm > 1000) {
                removable = removable.concat(k)
            }
        }
    })

    removable.forEach(k =>{
        if (inmemData[k]) {
            const pl = inmemData[k].player_list
            inmemData.playerIds = inmemData.playerIds.filter(id => !pl.includes(id)) // remove ids
            delete inmemData[k];
        }
    })
}


setInterval(clearRoom, 1000*1000)


const saveUnRep = (ip, port, msg) => {

}


const idSpace = 9

const randomNumber = () => Math.floor(Math.random() * Math.pow(10, idSpace))

const getPlayerId = () => {
    let result = null
    for (let i = 0; i < 10; i++) {
        result = randomNumber();

        if (!inmemData.includes(result)) {
            return result;
        }
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

    if (inmemData.playerIds.length > MAX_PLAYER) {
        clearRoom()
    }


    if (/.*create_room \d+.*/i.test(payload)) {
        const playerId = Number.parseInt(payload.split(' ')[1])
        inmemData.idMapIp[playerId] = address + ':' + port

        if (!inmemData.playerIds.includes(playerId)) {

            const roomId = nanoid();
            const room = {
                player_startpoint: DEFAULT_PLAYER_STARTPOINT,
                player_list: [playerId],
                status: 'queuing',
                lastModified: getCurrentSec()
                } // pending room

                inmemData.rooms[roomId] = room
                inmemData.playerIds.push(playerId)


                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                room.lastModified = getCurrentSec();

                const msg = JSON.stringify(room)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/^\d+ join_room.*$/i.test(payload)) {
            // try join room that have vacancy. If not have, create new and self join.
            // return the room joined
            console.log('joining')
            const playerId = Number.parseInt(payload.split(' ')[0])
            inmemData.idMapIp[playerId] = address + ':' + port

            if (inmemData.playerIds.includes(playerId)) {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                room.lastModified = getCurrentSec();

                const IPs = room.player_list.filter(x => x !== playerId).map(k => inmemData.idMapIp[k]).filter(x => x !== undefined)
                const player_list = room.player_list.filter(x => x !== playerId)

                const payload = {
                    ...room,
                    player_list: player_list,
                    IPs: IPs,
                    type: 4,
                    lastModified: room.lastModified
                }
                const msg = "register_details " + JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            } else {
                let room = Object.values(inmemData.rooms).find(x => x.player_list.length < x.player_startpoint && x.status !== 'playing')
                if (room) {
                    room.player_list.push(playerId)
                    inmemData.playerIds.push(playerId)
                } else {
                    const roomId = nanoid();
                    room = {
                        player_startpoint: DEFAULT_PLAYER_STARTPOINT,
                        player_list: [playerId],
                        status: 'queuing',
                    } // pending room

                    inmemData.rooms[roomId] = room
                    inmemData.playerIds.push(playerId)

                }
                // check if room ready, switch immediately
                if (room.player_list.length >= room.player_startpoint) {
                    room.status = 'playing'
                }
                
                room.lastModified = getCurrentSec();

                console.log(room.player_list.length, room.player_startpoint)

                const IPs = room.player_list.filter(x => x !== playerId).map(k => inmemData.idMapIp[k]).filter(x => x !== undefined)
                const player_list = room.player_list.filter(x => x !== playerId)
                const payload = {
                    ...room,
                    player_list: player_list,
                    IPs: IPs,
                    lastModified: room.lastModified
                }
                const msg = JSON.stringify(payload)
                server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
            }
        } else if (/.*list_room.*/i.test(payload)) {
            // list all rooms
            const msg = JSON.stringify(inmemData.rooms)
            server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))

        } else if (/.*\d+ delete_id .*/i.test(payload)) {
            // remove player's id
            const playerId = Number.parseInt(payload.split(' ')[0])
            if (inmemData.playerIds.includes(playerId)) {
                delete inmemData.idMapIp[playerId]

                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                room.lastModified = getCurrentSec();
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
            inmemData.idMapIp[playerId] = address + ':' + port

            if (inmemData.playerIds.includes(playerId)) {
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                if (room.player_list.length < room.player_startpoint && room.status === 'queuing') {
                    // few people
                    const payload = {
                        status: 2,
                        message: "Not enough people"
                    }
                    const msg = JSON.stringify(payload)
                    server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
                } else if (room.status === 'queuing' || room.status === 'playing') {
                    room.status = 'playing'
                    const IPs = room.player_list.filter(x => x !== playerId).map(k => inmemData.idMapIp[k]).filter(x => x !== undefined)
                    const player_list = room.player_list.filter(x => x !== playerId)
                    const payload = {
                        ...room,
                        player_list: player_list,
                        IPs: IPs
                    }
                    const msg = JSON.stringify(payload)
                    server.send(msg, 0, msg.length, port, address, (err) => !!err && console.warn(err))
                } else {
                    const payload = {
                        status: 3,
                        message: "Invalid room status"
                    }
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

            // delete the room that has the id of this player

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

        } else if (/^\d+ .*$/i.test(payload) && shouldForward) {
            console.log('t1')
            const playerId = Number.parseInt(payload.split(' ')[0])
            if (inmemData.playerIds.includes(playerId)) {
                console.log('t2')

                // player exists
                //now find room
                const room = Object.values(inmemData.rooms).find(x => x.player_list.includes(playerId))
                room.lastModified = getCurrentSec();

                const IPs = room.player_list.filter(x => x !== playerId).map(k => inmemData.idMapIp[k]).filter(x => x !== undefined)
                console.log('t3', IPs)

                IPs.forEach(ad => {
                    const [ip, p] = ad.split(':')
                    console.log('t5', ip, p, ad)
                    server.send(payload, 0, payload.length, p, ip, (err) => !!err && console.warn(err))
                })
            }
        }
    }
    );

server.bind(PORT);
