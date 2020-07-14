using Serilog;
using System;
using System.Threading.Tasks;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Events;

namespace TwitchChatVotingProxy.VotingReceiver
{
    /// <summary>
    /// Twitch voting receiver implementation
    /// </summary>
    class TwitchVotingReceiver : IVotingReceiver
    {
        public static readonly int RECONNECT_INTERVAL = 1000;

        public event EventHandler<OnMessageArgs> OnMessage;

        private TwitchClient client;
        private TwitchVotingReceiverConfig config;
        private ILogger logger = Log.Logger.ForContext<TwitchVotingReceiver>();

        public TwitchVotingReceiver(TwitchVotingReceiverConfig config)
        {
            this.config = config;

            // Connect to twitch
            logger.Information(
                $"trying to connect to channel \"{config.ChannelName}\" with user \"{config.UserName}\""
            );

            client = new TwitchClient(new WebSocketClient());
            client.Initialize(
                new ConnectionCredentials(config.UserName, config.OAuth),
                config.ChannelName
            );

            client.OnConnected += OnConnected;
            client.OnError += OnError;
            client.OnIncorrectLogin += OnIncorrectLogin;
            client.OnJoinedChannel += OnJoinedChannel;
            client.OnMessageReceived += OnMessageReceived;

            client.Connect();
        }

        public void SendMessage(string message)
        {
            try
            {
                client.SendMessage(config.ChannelName, message);
            } catch (Exception e)
            {
                logger.Error(e, $"failed to send message to channel \"{config.ChannelName}\"");
            }
        }
        /// <summary>
        /// Called when the twitch client connects (callback)
        /// </summary>
        private void OnConnected(object sender, OnConnectedArgs e)
        {
            logger.Information("successfully connected to twitch");
        }
        /// <summary>
        /// Called when the twitch client disconnects (callback)
        /// </summary>
        private async void OnDisconnect(object sender, OnDisconnectedArgs e)
        {
            logger.Error("disconnected from the twitch channel, trying to reconnect");
            await Task.Delay(RECONNECT_INTERVAL);
            client.Connect();
        }
        /// <summary>
        /// Called when the twitch clients errors (callback)
        /// </summary>
        private void OnError(object sender, OnErrorEventArgs e)
        {
            logger.Error(e.Exception, "client error, disconnecting");
            client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client has an failed login attempt (callback)
        /// </summary>
        private void OnIncorrectLogin(object sender, OnIncorrectLoginArgs e)
        {
            logger.Error("incorrect twitch login, check user name and oauth");
            client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client joins a channel (callback)
        /// </summary>
        private void OnJoinedChannel(object sender, OnJoinedChannelArgs e)
        {
            logger.Information($"successfully joined twitch channel \"{config.ChannelName}\"");
        }
        /// <summary>
        /// Called when the twitch client receives a message
        /// </summary>
        private void OnMessageReceived(object sender, OnMessageReceivedArgs e)
        {
            var chatMessage = e.ChatMessage;

            var evnt = new OnMessageArgs();
            evnt.Message = chatMessage.Message.Trim();
            evnt.ClientId = chatMessage.UserId;
            OnMessage.Invoke(this, evnt);
        }
    }
}
