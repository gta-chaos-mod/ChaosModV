const WEBPROXY_SERVER_URL = "http://127.0.0.1:45811";

const title = document.querySelector('#title');
let currentRoom = "";

const phase1 = document.querySelector('#phase-1');
const chaosToken = document.querySelector('#chaos-token');
const connectBtn = document.querySelector('#connect');

const phase2 = document.querySelector('#phase-2');
const votes = document.querySelector('#votes');
let voted = false;

const socket = io.connect(WEBPROXY_SERVER_URL);
socket.on('connect', function () {
    Toastify({
        text: "Connection to Chaos WebProxy Server established!",
        duration: 3000,
        close: true,
        backgroundColor: "linear-gradient(to right, #00b09b, #96c93d)"
    }).showToast();
});
socket.on('disconnect', function () {
    Toastify({
        text: "Lost Connection to Chaos WebProxy Server!",
        duration: 3000,
        close: true,
        backgroundColor: "linear-gradient(to right, #ff5f6d, #ffc371)"
    }).showToast();
});

socket.on('room:join-state', function (state) {
    switch(state) {
        case 0:
            phase1.style.display = 'none';
            phase2.style.display = 'block';
            title.innerHTML = "Connected to Chaos-Room: " + currentRoom;
            break;
        case 1:
            Toastify({
                text: "Could not connect to Room!",
                duration: 3000,
                close: true,
                backgroundColor: "linear-gradient(to right, #ff5f6d, #ffc371)"
            }).showToast();
            break;
    }
    connectBtn.disabled = false;
});

socket.on('game:new', function (votesList) {
    voted = false;
    votes.innerHTML = "";
    for(let i = 0; i < votesList.length; i++) {
        let vote = votesList[i];
        votes.innerHTML += `<div class="vote" onclick="selectVote(this, ${i})">${vote.Label}</div>`;
    }
});

function selectVote(element, i) {
    if(voted) return;
    voted = true;
    let voteBoxes = document.querySelectorAll('.vote');
    for(let j = 0; j < voteBoxes.length; j++) {
        voteBoxes[j].classList.add('not');
    }
    element.classList.remove('not');
    element.classList.add('active');
    socket.emit('game:vote', currentRoom, i);
}

connectBtn.onclick = function () {
    connectBtn.disabled = true;
    currentRoom = chaosToken.value;
    socket.emit('room:join', chaosToken.value);
};