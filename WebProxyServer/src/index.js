require('dotenv').config();

const express = require('express');

const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const roomOwners = {};
const ownerClients = {};
const roomIds = [];
const currentVotes = {};

function makeid(length) {
    var result           = '';
    var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    var charactersLength = characters.length;
    for ( var i = 0; i < length; i++ ) {
       result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
 }

function generateRoomId() {
    let uuid;
    while(roomIds.includes(uuid = makeid(6))) {}
    return uuid.split('-').join('').toUpperCase();
}

io.on('connection', (socket) => {

    socket.on('room:request', function () {
        let roomId = generateRoomId();
        roomIds.push(roomId);
        ownerClients[roomId] = socket;
        roomOwners[socket.id] = roomId;
        console.log("room setup!");
        socket.emit('room:send', roomId);
    });

    socket.on('room:join', function (roomId) {
        console.log(roomIds);
        if(!roomIds.includes(roomId)) {
            socket.emit('room:join-state', 1);
            return;
        }
        socket.join(roomId);
        socket.emit('room:join-state', 0);
        if(currentVotes[roomId])
            socket.emit('game:new', currentVotes[roomId]);
    });

    socket.on('game:new', function (room, votes) {
        currentVotes[room] = votes;
        io.sockets.in(room).emit('game:new', votes);
    });

    socket.on('game:vote', function (room, voteIdx) {
        if(ownerClients[room])
            ownerClients[room].emit('game:vote', socket.id, voteIdx);
    });

    socket.on('disconnect', function () {
        if(!roomOwners[socket.id]) return;
        console.log("room destroy!");
        let roomId = roomOwners[socket.id];
        let idx = roomIds.indexOf(roomId);
        roomIds.splice(idx, 1);
        delete currentVotes[roomId];
        delete ownerClients[roomId];
        delete roomOwners[socket.id];
    });
});

app.use(express.static(__dirname + "/../public"));

const port = 45811;
http.listen(port, function () {
    console.log("Master Server running on *:" + port);
});