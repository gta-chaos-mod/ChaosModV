using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Events;

namespace TwitchChatVotingProxy.Properties
{
    class TwitchVoting
    {
        public static readonly int RECONNECT_INTERVAL = 1000;
        public static readonly int DISPLAY_UPDATE_INTEVAL = 200;

        public List<IVoteOption> ActiveVoteOptions { get; set; }

        private Timer displayUpdateInterval = new Timer();
        private List<string> usersThatVoted = new List<string>();
        private IDisplayWebsocket displayWebsocket = new DisplayWebsocket("ws://127.0.0.1:9091");

        public TwitchVoting(string channnleName, string oAuth)
        {
            // Initialize twitch client
            TwitchClient twitchClient = new TwitchClient(new WebSocketClient());
            twitchClient.Initialize(new ConnectionCredentials(channnleName, oAuth), channnleName);
            // Set twitch connection error handler
            twitchClient.OnIncorrectLogin += (object sender, OnIncorrectLoginArgs e) =>
            {
                Console.Error.WriteLine("Twitch client incorrect login");
            };
            twitchClient.OnConnectionError += (object sender, OnConnectionErrorArgs e) =>
            {
                Console.Error.WriteLine("Twitch connection failed");
                Console.Error.WriteLine(e);
            };
            // Set twitch error handler
            twitchClient.OnError += (object sender, OnErrorEventArgs e) =>
            {
                Console.Error.WriteLine("Twitch client error, disconnecting");
                Console.Error.WriteLine(e);
                twitchClient.Disconnect();
            };
            // Set twtich channle join handler
            twitchClient.OnJoinedChannel += (object sender, OnJoinedChannelArgs e) =>
            {
                Console.WriteLine("Sucessfully joined the channel");
                // Start the display update interval
                Timer displayUpdateTimer = new Timer();
                displayUpdateTimer.Interval = 250;
                displayUpdateTimer.Elapsed += this.SendDisplayUpdate;
                displayUpdateTimer.Start();
            };
            twitchClient.OnDisconnected += (object sender, OnDisconnectedEventArgs e) =>
            {
                Console.WriteLine("Twtich client disconnected, reconnecting");
                Task.Run(async () =>
                {
                    await Task.Delay(RECONNECT_INTERVAL);
                    twitchClient.Connect();
                });
            };
            // Set twitch message handler
            twitchClient.OnMessageReceived += OnTwichMessage;
            // Connect to twitch
            twitchClient.Connect();
            // Setup display update interval
            displayUpdateInterval.Interval = DISPLAY_UPDATE_INTEVAL;
            displayUpdateInterval.Elapsed += SendDisplayUpdate;
            displayUpdateInterval.Enabled = true;
        }

        public void createVote(List<IVoteOption> voteOptions)
        {
            ActiveVoteOptions = voteOptions;
            displayWebsocket.createVote(voteOptions);
            // Clear the user that voted so they can vote again
            usersThatVoted.Clear();
        }

        private void OnTwichMessage(object sender, OnMessageReceivedArgs e)
        {
            string userId = e.ChatMessage.UserId;
            string message = e.ChatMessage.Message;

            Console.WriteLine(message);

            // Loop through all possible vote options
            foreach (IVoteOption voteOption in ActiveVoteOptions)
            {
                
                if (voteOption.Matches.Contains(message) && !usersThatVoted.Contains(userId))
                {
                    usersThatVoted.Add(userId);
                    voteOption.Votes++;
                }
                Console.WriteLine($"Option: {voteOption.Label}, Votes: {voteOption.Votes}");
            }
        }

        private void SendDisplayUpdate(object source, ElapsedEventArgs e)
        {
            displayWebsocket.updateVotes(ActiveVoteOptions);
        }
    }
}
