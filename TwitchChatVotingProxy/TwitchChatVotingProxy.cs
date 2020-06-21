using Fleck;
using Newtonsoft.Json;
using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Threading.Tasks;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        private static StreamReader m_streamReader;
        private static StreamWriter m_streamWriter;
        private static TwitchClient m_twitchClient;
        private static string m_twitchChannelName;
        private static bool m_twitchPollMode = false;
        private static int m_twitchPollDur;
        private static string m_twitchPollUUID = null;
        private static WebSocketServer m_twitchSocketServer;
        private static List<IWebSocketConnection> m_twitchPollClients;
        private static bool m_voteRunning = false;
        private static int[] m_votes = new int[3];
        private static List<VoteUser> m_alreadyVotedUsers = new List<VoteUser>();
        private static bool m_disableNoVoteMsg = false;
        private static bool m_alternatedVotingRound;
        private static bool m_enableTwitchChanceSystem;
        private static Random m_random = new Random();

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
                Console.WriteLine("Error whilst connecting to pipe!");

                return;
            }

            Console.WriteLine("Connected to pipe!");

            m_streamReader = new StreamReader(pipe);
            m_streamWriter = new StreamWriter(pipe);
            m_streamWriter.AutoFlush = true;

            Task<bool> loginTask = TwitchLogin();
            while (!loginTask.IsCompleted)
            {

            }

            if (!loginTask.Result)
            {
                return;
            }

            while (pipe.IsConnected)
            {
                PipeStreamReadTick();
                PipeStreamWriteTick();
            }
        }

        private static async Task<bool> TwitchLogin()
        {
            string twitchUsername = null;
            string twitchOAuth = null;
            string twitchPollPass = null;

            m_twitchPollMode = File.Exists("chaosmod/.twitchpoll");

            OptionsFile twitchFile = new OptionsFile("chaosmod/twitch.ini");
            twitchFile.ReadFile();

            m_twitchChannelName = twitchFile.ReadValue("TwitchUserName");
            twitchUsername = twitchFile.ReadValue("TwitchChannelName");
            twitchOAuth = twitchFile.ReadValue("TwitchChannelOAuth");
            twitchPollPass = twitchFile.ReadValue("TwitchVotingPollPass");
            m_disableNoVoteMsg = twitchFile.ReadValueBool("TwitchVotingDisableNoVoteRoundMsg", false);
            m_enableTwitchChanceSystem = twitchFile.ReadValueBool("TwitchVotingChanceSystem", false);

            if (m_twitchPollMode)
            {
                OptionsFile configFile = new OptionsFile("chaosmod/config.ini");
                configFile.ReadFile();

                m_twitchPollDur = twitchFile.ReadValueInt("NewEffectSpawnTime", 30);

                if (m_twitchPollDur < 15 || m_twitchPollDur > 180)
                {
                    SendToPipe("invalid_poll_dur");

                    return false;
                }
            }

            if (m_twitchPollMode)
            {
                m_twitchPollClients = new List<IWebSocketConnection>();

                m_twitchSocketServer = new WebSocketServer("ws://0.0.0.0:31337");
                m_twitchSocketServer.RestartAfterListenError = true;
                m_twitchSocketServer.Start(socket =>
                {
                    socket.OnOpen += () =>
                    {
                        Console.WriteLine($"New client! ({socket.ConnectionInfo.ClientIpAddress}:{socket.ConnectionInfo.ClientPort})");

                        socket.Send($"{{\"type\":\"auth\",\"data\":\"{twitchPollPass}\"}}");

                        m_twitchPollClients.Add(socket);
                    };

                    socket.OnMessage += (msg) =>
                    {
                        Console.WriteLine(msg);

                        dynamic json = DeserializeJson(msg);
                        if (json == null)
                        {
                            return;
                        }

                        string type = json.type;

                        if (type == "created")
                        {
                            m_twitchPollUUID = json.id;
                        }
                        else if (type == "update")
                        {
                            dynamic choices = json.poll.choices;

                            m_votes[0] = (int)choices[0].votes;
                            m_votes[1] = (int)choices[1].votes;
                            m_votes[2] = (int)choices[2].votes;

                            m_twitchPollUUID = null;
                        }
                    };

                    socket.OnClose += () =>
                    {
                        Console.WriteLine($"Connection to client {socket.ConnectionInfo.ClientIpAddress}:{socket.ConnectionInfo.ClientPort} closed.");

                        m_twitchPollClients.Remove(socket);
                    };
                });

                return true;
            }

            if (string.IsNullOrWhiteSpace(m_twitchChannelName) || string.IsNullOrWhiteSpace(twitchUsername) || string.IsNullOrWhiteSpace(twitchOAuth))
            {
                SendToPipe("invalid_login");

                return false;
            }

            ConnectionCredentials credentials = new ConnectionCredentials(twitchUsername, twitchOAuth);
            WebSocketClient webSocketClient = new WebSocketClient();

            m_twitchClient = new TwitchClient(webSocketClient);
            m_twitchClient.Initialize(credentials, m_twitchChannelName);

            m_twitchClient.OnMessageReceived += OnMessageRecieved;

            bool failed = false;
            bool done = false;

            m_twitchClient.OnConnectionError += (object sender, OnConnectionErrorArgs e) =>
            {
                failed = true;
                done = true;
            };

            m_twitchClient.OnConnected += (object sender, OnConnectedArgs e) =>
            {
                done = true;
            };

            m_twitchClient.Connect();

            int lastTick = Environment.TickCount;
            while (!done)
            {
                await Task.Delay(100);

                if (lastTick < Environment.TickCount - 3000)
                {
                    failed = true;
                    done = true;

                    break;
                }
            }

            if (failed)
            {
                SendToPipe("invalid_login");

                return false;
            }

            Console.WriteLine("Logged into Twitch Account!");

            done = false;

            m_twitchClient.OnJoinedChannel += (object sender, OnJoinedChannelArgs e) =>
            {
                if (e.Channel.ToLower() == m_twitchChannelName.ToLower())
                {
                    done = true;
                }
            };

            lastTick = Environment.TickCount;
            while (!done)
            {
                await Task.Delay(100);

                if (lastTick < Environment.TickCount - 1500)
                {
                    failed = true;
                    done = true;
                }
            }

            if (failed)
            {
                SendToPipe("invalid_channel");

                return false;
            }

            Console.WriteLine("Connected to Twitch Channel!");

            return true;
        }

        private static void OnMessageRecieved(object sender, OnMessageReceivedArgs e)
        {
            if (!m_voteRunning)
            {
                return;
            }

            ChatMessage chatMessage = e.ChatMessage;
            string userId = chatMessage.UserId;

            string msg = chatMessage.Message.Trim();
            if (msg.Length > 1)
            {
                return;
            }

            VoteUser voteUser = m_alreadyVotedUsers.Where(user => user.UserId == userId).FirstOrDefault();
            if (voteUser == null)
            {
                voteUser = new VoteUser(userId);

                m_alreadyVotedUsers.Add(voteUser);
            }

            int votedFor = -1;
            if (msg == (!m_alternatedVotingRound ? "1" : "4"))
            {
                votedFor = 0;
            }
            else if (msg == (!m_alternatedVotingRound ? "2" : "5"))
            {
                votedFor = 1;
            }
            else if (msg == (!m_alternatedVotingRound ? "3" : "6"))
            {
                votedFor = 2;
            }

            if (votedFor != -1)
            {
                if (voteUser.VotedForIndex != -1)
                {
                    // This user already voted before, remove their previous vote
                    m_votes[voteUser.VotedForIndex]--;
                }
                voteUser.VotedForIndex = votedFor;

                m_votes[votedFor]++;
            }
        }

        static Task<string> _LineReadTask = null;
        private static void PipeStreamReadTick()
        {
            if (_LineReadTask == null)
            {
                _LineReadTask = m_streamReader.ReadLineAsync();
            }
            else if (_LineReadTask.IsCompleted)
            {
                string line = _LineReadTask.Result;
                _LineReadTask = null;

                Console.WriteLine(line);

                if (line.StartsWith("vote:"))
                {
                    if (m_voteRunning)
                    {
                        return;
                    }

                    string[] data = line.Split(':');

                    m_votes[0] = 0;
                    m_votes[1] = 0;
                    m_votes[2] = 0;
                    m_alreadyVotedUsers.Clear();
                    m_voteRunning = true;

                    if (m_twitchPollMode)
                    {
                        SendPollJson($"{{\"type\":\"create\",\"title\":\"[Chaos Mod V] Next Effect Vote!\",\"duration\":{m_twitchPollDur}," +
                            $"\"choices\":[\"{data[1]}\",\"{data[2]}\",\"{data[3]}\"]}}");
                    }
                    else
                    {
                        m_twitchClient.SendMessage(m_twitchChannelName, "Time for a new effect! Vote between:");

                        m_alternatedVotingRound = int.Parse(data[4]) != 0 ? true : false;
                        if (!m_alternatedVotingRound)
                        {
                            m_twitchClient.SendMessage(m_twitchChannelName, $"1: {data[1]}");
                            m_twitchClient.SendMessage(m_twitchChannelName, $"2: {data[2]}");
                            m_twitchClient.SendMessage(m_twitchChannelName, $"3: {data[3]}");
                        }
                        else
                        {
                            m_twitchClient.SendMessage(m_twitchChannelName, $"4: {data[1]}");
                            m_twitchClient.SendMessage(m_twitchChannelName, $"5: {data[2]}");
                            m_twitchClient.SendMessage(m_twitchChannelName, $"6: {data[3]}");
                        }

                        if (m_enableTwitchChanceSystem)
                        {
                            m_twitchClient.SendMessage(m_twitchChannelName, "Votes will affect the chance for one of the effects to occur.");
                        }
                    }
                }
                else if (line == "getvoteresult")
                {
                    if (!m_voteRunning)
                    {
                        return;
                    }

                    if (m_twitchPollMode && m_twitchPollUUID != null)
                    {
                        SendPollJson($"{{\"type\":\"end\",\"id\":\"{m_twitchPollUUID}\"}}");
                    }

                    SendToPipe($"voteresult:{GetResultEffect()}");

                    m_voteRunning = false;
                }
                else if (line == "getcurrentvotes")
                {
                    if (m_voteRunning)
                    {
                        SendToPipe($"currentvotes:{m_votes[0]}:{m_votes[1]}:{m_votes[2]}");
                    }
                }
                else if (line == "novoteround" && !m_twitchPollMode && !m_disableNoVoteMsg)
                {
                    m_twitchClient.SendMessage(m_twitchChannelName, "No voting this time! Chaos Mod will decide for an effect itself.");
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

                SendToPipe("ping");
            }
        }

        private static void SendToPipe(string msg)
        {
            m_streamWriter.Write(msg + '\0');
        }

        private static dynamic DeserializeJson(string json)
        {
            return (dynamic)JsonConvert.DeserializeObject(json);
        }

        private static void SendPollJson(string json)
        {
            foreach (IWebSocketConnection client in m_twitchPollClients)
            {
                client.Send(json);
            }
        }

        private static int GetResultEffect()
        {
            if (!m_enableTwitchChanceSystem)
            {
                List<int> chosenEffects = new List<int>();
                int highestVotes = 0;
                for (int i = 0; i < 3; i++)
                {
                    int votes = m_votes[i];
                    if (votes > highestVotes)
                    {
                        chosenEffects.Clear();
                        chosenEffects.Add(i);

                        highestVotes = votes;
                    }
                    else if (votes == highestVotes)
                    {
                        chosenEffects.Add(i);
                    }
                }

                int count = chosenEffects.Count;
                if (count > 1)
                {
                    // Multiple effects with same amount of votes, choose random one
                    int chosen = chosenEffects[m_random.Next(0, count)];
                    chosenEffects.Clear();
                    chosenEffects.Add(chosen);
                }

                return chosenEffects[0];
            }
            else
            {
                int totalVotes = m_votes[0] + m_votes[1] + m_votes[2];

                if (totalVotes == 0)
                {
                    // No votes, just choose a random one
                    return m_random.Next(0, 4);
                }
                else
                {
                    int index = m_random.Next(0, totalVotes + 1);

                    int addedUpVotes = 0;
                    for (int i = 0; i < 3; i++)
                    {
                        if (m_votes[i] == 0)
                        {
                            continue;
                        }

                        addedUpVotes += m_votes[i];

                        if (index <= addedUpVotes)
                        {
                            return i;
                        }
                    }
                }

                // This should literally never happen, but w/e just return the first voteable effect
                return 0;
            }
        }
    }
}