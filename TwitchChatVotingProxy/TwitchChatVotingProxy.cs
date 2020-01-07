using System;
using System.IO;
using System.IO.Pipes;
using System.Threading.Tasks;
using TwitchLib.Client;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        static StreamReader _StreamReader;
        static StreamWriter _StreamWriter;
        static TwitchClient _TwitchClient;

        static void Main(string[] args)
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

        static bool TwitchLogin()
        {
            string twitchChannelName = null;
            string twitchUserName = null;
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
                    case "twitchUserName":
                        twitchUserName = text[1];
                        break;
                    case "TwitchChannelOAuth":
                        twitchOAuth = text[1];
                        break;
                }
            }

            if (twitchChannelName == null || twitchUserName == null || twitchOAuth == null)
            {
                _StreamWriter.Write("invalid_login\0");

                return false;
            }

            ConnectionCredentials credentials = new ConnectionCredentials(twitchUserName, twitchOAuth);
            WebSocketClient webSocketClient = new WebSocketClient();

            _TwitchClient = new TwitchClient(webSocketClient);
            _TwitchClient.Initialize(credentials, twitchChannelName);
            
            if (!_TwitchClient.IsConnected)
            {
                _StreamWriter.Write("invalid_login\0");

                return false;
            }

            return true;
        }

        static Task<string> _LineReadTask = null;
        static void PipeStreamReadTick()
        {
            if (_LineReadTask == null)
            {
                _LineReadTask = _StreamReader.ReadLineAsync();
            }
            else if (_LineReadTask.Status == TaskStatus.RanToCompletion)
            {
                string line = _LineReadTask.Result;
                _LineReadTask = null;

                Console.WriteLine("Read line!");
            }
        }

        static int _LastTick = Environment.TickCount;
        static void PipeStreamWriteTick()
        {
            int curTick = Environment.TickCount;

            if (_LastTick < curTick - 1000)
            {
                _LastTick = curTick;

                _StreamWriter.Write("ping\0");

                Console.WriteLine("Sent ping!");
            }
        }
    }
}