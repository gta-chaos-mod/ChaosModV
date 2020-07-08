using Serilog;
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
        private IOverlayServer displayWebsocket = new OverlayServer("ws://127.0.0.1:9091", VotingMode.MAJORITY);

        public TwitchVoting(string channelName, string oAuth)
        {
            // Initialize twitch client
            TwitchClient twitchClient = new TwitchClient(new WebSocketClient());
            Log.Logger.Information($"trying to connect to channel \"{channelName}\"");
            twitchClient.Initialize(new ConnectionCredentials(channelName, oAuth), channelName);
            // Set twitch connection error handler
            twitchClient.OnIncorrectLogin += (object sender, OnIncorrectLoginArgs e) =>
            {
                Log.Logger.Error("twitch client incorrect login");
            };
            twitchClient.OnConnectionError += (object sender, OnConnectionErrorArgs e) =>
            {
                Log.Logger.Error(e.Error.Message, "twitch client connection error");
            };
            // Set twitch error handler
            twitchClient.OnError += (object sender, OnErrorEventArgs e) =>
            {
                Log.Logger.Error(e.Exception, "twitch client error, disconnecting");
                twitchClient.Disconnect();
            };
            // Set twtich channle join handler
            twitchClient.OnJoinedChannel += (object sender, OnJoinedChannelArgs e) =>
            {
                Log.Logger.Information("twitch client joined channel");
                // Start the display update interval
                Timer displayUpdateTimer = new Timer();
                displayUpdateTimer.Interval = 250;
                displayUpdateTimer.Elapsed += this.SendDisplayUpdate;
                displayUpdateTimer.Start();
            };
            twitchClient.OnFailureToReceiveJoinConfirmation += (object sender, OnFailureToReceiveJoinConfirmationArgs e) =>
            {
                Log.Logger.Error($"failed to join twitch channel \"{channelName}\"");
            };
            twitchClient.OnDisconnected += (object sender, OnDisconnectedEventArgs e) =>
            {
                Log.Logger.Error("twitch client disconnected, reconnecting");
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
        public void CreateVote(List<IVoteOption> voteOptions)
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

            // Loop through all possible vote options
            foreach (IVoteOption voteOption in ActiveVoteOptions)
            {
                
                if (voteOption.Matches.Contains(message) && !usersThatVoted.Contains(userId))
                {
                    usersThatVoted.Add(userId);
                    voteOption.Votes++;
                }
            }
        }
        private void SendDisplayUpdate(object source, ElapsedEventArgs e)
        {
            displayWebsocket.updateVotes(ActiveVoteOptions);
        }
    }
}
