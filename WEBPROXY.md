# Web Proxy
The Web Proxy is a system like the Twitch Proxy System, just for the Web Browser. There are 3 main components: The ChaosMod with integrated WebProxy Pipeline (copied from the Twitch Pipeline), the Web Proxy Console Application and the NodeJS SocketIO Server with integrated Express Web Panel.

Currently the Web Proxy does not support any kind of Configuration, but this will change later on - hopefully.

## Setup and Usage
The WebProxy Console connects via IPC to the ChaosMod like the Twitch Proxy does. The Proxy also connects to a master SocketIO Server. The SocketIO Server holds the Web Panel through which the enemy players can vote.

The Master Server should run on a remote machine which supports NodeJS. When the Game is started, just run the WebProxy Console. After that, re-enable the ChaosMod (by CTRL+L). The Console should print out a *Chaos-Token* this token can be used to connect to the Voting-Session in the Web Panel. Now it should just work like the Twitch Voting System.