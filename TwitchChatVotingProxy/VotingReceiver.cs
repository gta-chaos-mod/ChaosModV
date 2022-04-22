using Serilog;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using VotingProxy.VotingChatClient;
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
        private VotingReceiverConfig config;
        private ILogger logger = Log.Logger.ForContext<ChatVotingReceiver>();

        private string GetName()
        {
            return config.ChannelType == EChannelType.TWITCH ? "twitch" : "discord";
        }

        public ChatVotingReceiver(VotingReceiverConfig config)
        {
            this.config = config;

            // Connect to channel
            logger.Information(
                $"trying to connect to channel \"{config.ChannelId}\" with user \"{config.UserId}\""
            );

            if (config.ChannelType == EChannelType.TWITCH)
            {
                logger.Information("init twit");
                client = new TwitchChatClient();
                client.Initialize(new Credentials(config.ChannelId, config.UserId, config.OAuth));
            }
            else if (config.ChannelType == EChannelType.DISCORD)
            {
                logger.Information("init disc");
                client = new DiscordChatClient();
                client.Initialize(new Credentials(config.ChannelId, config.UserId, config.OAuth));
            }

            

            client.OnConnected += OnConnected;
            client.OnError += OnError;
            client.OnIncorrectLogin += OnIncorrectLogin;
            client.OnJoinedChannel += OnJoinedChannel;
            client.OnMessageReceived += OnMessageReceived;

            client.Connect();
        }

        public void SendMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            try
            {
                client.SendMessage(options, votingMode, config.ChannelId);
            } catch (Exception e)
            {
                logger.Error(e, $"failed to send message to channel \"{config.ChannelId}\"");
            }
        }

        public void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            try
            {
                client.UpdateMessage(options, votingMode, config.ChannelId);
            }
            catch (Exception e)
            {
                logger.Error(e, $"failed to update message to channel \"{config.ChannelId}\"");
            }
        }
        /// <summary>
        /// Called when the twitch client connects (callback)
        /// </summary>
        private void OnConnected(object sender, EventArgs e)
        {
            logger.Information($"successfully connected to {GetName()}");
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
            logger.Error($"incorrect login, check user name and oauth. ({GetName()})");
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
}
