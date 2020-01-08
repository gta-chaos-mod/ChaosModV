using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;
using TwitchLib.Api.Models.v5.Channels;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Models;

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        static StreamReader _StreamReader;
        static StreamWriter _StreamWriter;
        static TwitchClient _TwitchClient;

        private static void Main(string[] args)
        {
            NamedPipeClientStream pipe = new NamedPipeClientStream(".", "ChaosModVTwitchChatPipe", PipeDirection.InOut, PipeOptions.Asynchronous);
            try
            {
                pipe.Connect(1000);
            }
            catch (IOException)
            {

            }
            catch (TimeoutException)
            {

            }

            if (!pipe.IsConnected)
            {
                Console.WriteLine("Error while connecting to pipe!");

                return;
            }

            Console.WriteLine("Connected to pipe!");

            _StreamReader = new StreamReader(pipe);
            _StreamWriter = new StreamWriter(pipe);
            _StreamWriter.AutoFlush = true;

            if (!TwitchLogin())
            {
                return;
            }

            while (pipe.IsConnected)
            {
                PipeStreamReadTick();
                PipeStreamWriteTick();
            }
        }

        private static bool TwitchLogin()
        {
            string twitchChannelName = null;
            string twitchUsername = null;
            string twitchOAuth = null;

            string data = File.ReadAllText("chaosmod/config.ini");
            foreach (string line in data.Split('\n'))
            {
                string[] text = line.Split('=');
                if (text.Length < 2)
                {
                    continue;
                }

                switch (text[0])
                {
                    case "TwitchChannelName":
                        twitchChannelName = text[1];
                        break;
                    case "TwitchUserName":
                        twitchUsername = text[1];
                        break;
                    case "TwitchChannelOAuth":
                        twitchOAuth = text[1];
                        break;
                }
            }

            if (twitchChannelName == null || twitchUsername == null || twitchOAuth == null)
            {
                _StreamWriter.Write("invalid_login\0");

                return false;
            }

            ConnectionCredentials credentials = new ConnectionCredentials(twitchUsername, twitchOAuth);
            WebSocketClient webSocketClient = new WebSocketClient();

            _TwitchClient = new TwitchClient(webSocketClient);
            _TwitchClient.AutoReListenOnException = true;
            _TwitchClient.Initialize(credentials, twitchChannelName);

            _TwitchClient.OnMessageReceived += OnMessageRecieved;

            _TwitchClient.Connect();
            
            if (!_TwitchClient.IsConnected)
            {
                _StreamWriter.Write("invalid_login\0");

                return false;
            }

            return true;
        }

        private static void OnMessageRecieved(object sender, OnMessageReceivedArgs e)
        {
            
        }

        static Task<string> _LineReadTask = null;
        private static void PipeStreamReadTick()
        {
            if (_LineReadTask == null)
            {
                _LineReadTask = _StreamReader.ReadLineAsync();
            }
            else if (_LineReadTask.IsCompleted)
            {
                string line = _LineReadTask.Result;
                _LineReadTask = null;

                if (line.StartsWith("vote:"))
                {
                    string[] data = line.Split(':');

                    _TwitchClient.SendMessage(_TwitchClient.JoinedChannels[0], "Time for a new effect! Vote between:");
                    Thread.Sleep(1000);
                    _TwitchClient.SendMessage(_TwitchClient.JoinedChannels[0], $"1: {data[1]}");
                    Thread.Sleep(1000);
                    _TwitchClient.SendMessage(_TwitchClient.JoinedChannels[0], $"2: {data[2]}");
                    Thread.Sleep(1000);
                    _TwitchClient.SendMessage(_TwitchClient.JoinedChannels[0], $"3: {data[3]}");
                }
            }
        }

        static int _LastTick = Environment.TickCount;
        private static void PipeStreamWriteTick()
        {
            int curTick = Environment.TickCount;

            if (_LastTick < curTick - 1000)
            {
                _LastTick = curTick;

                _StreamWriter.Write("ping\0");
            }
        }
    }
}