using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TwitchChatVotingProxy.TwitchConfig;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Interfaces;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Events;

namespace TwitchChatVotingProxy.VotingReceiver
{
    class TwitchVotingReceiver : IVotingReceiver
    {
        public static readonly int RECONNECT_INTERVAL = 1000;

        public event EventHandler<OnMessageArgs> OnMessage;

        private TwitchClient client;
        private ITwtichConfig config;
        private ILogger logger = Log.Logger.ForContext<TwitchVotingReceiver>();

        public TwitchVotingReceiver(ITwtichConfig config)
        {
            this.config = config;

            // Connect to twitch
            logger.Information(
                $"trying to connect to channel \"{config.ChannelName}\" with user \"{config.UserName}\""
            );

            client = new TwitchClient(new WebSocketClient());
            client.Initialize(new ConnectionCredentials(config.UserName, config.OAuth), config.ChannelName);

            client.OnConnected += OnConnected;
            client.OnError += OnError;
            client.OnIncorrectLogin += OnIncorrectLogin;
            client.OnJoinedChannel += OnJoinedChannel;
            client.OnMessageReceived += OnMessageReceived;

            client.Connect();
        }

        private void OnConnected(object sender, OnConnectedArgs e)
        {
            logger.Information("successfully connected to twitch");
        }
        private void OnError(object sender, OnErrorEventArgs e)
        {
            logger.Error(e.Exception, "client error, disconnecting");
            client.Disconnect();
        }
        private void OnIncorrectLogin(object sender, OnIncorrectLoginArgs e)
        {
            logger.Error("incorrect twitch login, check user name and oauth");
            client.Disconnect();
        }
        private void OnJoinedChannel(object sender, OnJoinedChannelArgs e)
        {
            logger.Information($"successfully joined twitch channel \"{config.ChannelName}\"");
        }
        private void OnMessageReceived(object sender, OnMessageReceivedArgs e)
        {
            OnMessage.Invoke(this, new OnMessageArgs(e.ChatMessage.Message.Trim()));
        }
    }
}
