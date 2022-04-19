using Serilog;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using VotingProxy.VotingChatClient;
using VotingProxy.VotingDiscordClient;
using TwitchLib.Communication.Clients;

namespace VotingProxy.VotingReceiver
{
    /// <summary>
    /// Twitch Voting receiver implementation
    /// </summary>
    class ChatVotingReceiver : IVotingReceiver
    {
        public static readonly int RECONNECT_INTERVAL = 1000;

        public event EventHandler<OnMessageArgs> OnMessage;

        private ChatClient client;
        private TwitchVotingReceiverConfig config;
        private ILogger logger = Log.Logger.ForContext<ChatVotingReceiver>();

        public ChatVotingReceiver(TwitchVotingReceiverConfig config)
        {
            this.config = config;

            // Connect to channel
            logger.Information(
                $"trying to connect to channel \"{config.ChannelId}\" with user \"{config.UserName}\""
            );

            if (config.ChannelType == EChannelType.TWITCH)
            {
                client = new TwitchChatClient();
            }

            client.Initialize(
                new Credentials(config.UserName, config.OAuth),
                config.ChannelId
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
                client.SendMessage(config.ChannelId, message);
            } catch (Exception e)
            {
                logger.Error(e, $"failed to send message to channel \"{config.ChannelId}\"");
            }
        }
        /// <summary>
        /// Called when the twitch client connects (callback)
        /// </summary>
        private void OnConnected(object sender, EventArgs e)
        {
            logger.Information("successfully connected to twitch");
        }
        /// <summary>
        /// Called when the twitch client disconnects (callback)
        /// </summary>
        private async void OnDisconnect(object sender, EventArgs e)
        {
            logger.Error("disconnected from the channel, trying to reconnect");
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
        private void OnIncorrectLogin(object sender, EventArgs e)
        {
            logger.Error("incorrect login, check user name and oauth");
            client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client joins a channel (callback)
        /// </summary>
        private void OnJoinedChannel(object sender, EventArgs e)
        {
            logger.Information($"successfully joined channel \"{config.ChannelId}\"");
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

    /// <summary>
    /// Discord Voting receiver implementation
    /// </summary>
    class DiscordVotingReceiver : IVotingReceiverDiscord
    {
        public static readonly int RECONNECT_INTERVAL = 1000;

        public event EventHandler<OnDiscordMessageArgs> OnDiscordMessage;

        private DiscordClient client;
        private DiscordVotingRecieverConfig config;
        private ILogger logger = Log.Logger.ForContext<ChatVotingReceiver>();

        public DiscordVotingReceiver(DiscordVotingRecieverConfig config)
        {
            this.config = config;

            // Connect to channel
            logger.Information(
                $"trying to connect to guild \"{config.GuildID}\" with token \"{config.Token}\""
            );

            client = new DiscordChatClient(new DiscordCredentials(config.ChannelId, config.GuildID, config.Token));

            client.OnDiscordConnected += OnDiscordConnected;
            client.OnDiscordIncorrectLogin += OnDiscordIncorrectLogin;
            client.OnDiscordMessageReceived += OnDiscordMessageReceived;

            client.Connect();
        }

        public void SendMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            try
            {
                client.SendMessage(options, votingMode);
            }
            catch (Exception e)
            {
                logger.Error(e, $"failed to send message to channel \"{config.ChannelId}\"");
            }
        }
        /// <summary>
        /// Called when the discord client connects (callback)
        /// </summary>
        private void OnDiscordConnected(object sender, EventArgs e)
        {
            logger.Information("successfully connected to twitch");
        }
        /// <summary>
        /// Called when the discord client disconnects (callback)
        /// </summary>
        private async void OnDiscordDisconnect(object sender, EventArgs e)
        {
            logger.Error("disconnected from the channel, trying to reconnect");
            await Task.Delay(RECONNECT_INTERVAL);
            client.Connect();
        }

        /// <summary>
        /// Called when the discord client has an failed login attempt (callback)
        /// </summary>
        private void OnDiscordIncorrectLogin(object sender, EventArgs e)
        {
            logger.Error("incorrect login, check user name and oauth");
            client.Disconnect();
        }
        /// <summary>
        /// Called when the discord client receives a message
        /// </summary>
        private void OnDiscordMessageReceived(object sender, OnDiscordMessageReceivedArgs e)
        {
            var chatMessage = e.ChatMessage;

            var evnt = new OnDiscordMessageArgs();
            evnt.Message = chatMessage.Message.Trim();
            evnt.ClientId = chatMessage.UserId;
            OnDiscordMessage.Invoke(this, evnt);
        }
    }
}
