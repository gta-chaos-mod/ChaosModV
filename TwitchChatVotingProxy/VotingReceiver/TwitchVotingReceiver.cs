using Serilog;
using System;
using System.Threading.Tasks;
using TwitchChatVotingProxy.Config;
using TwitchLib.Client;
using TwitchLib.Client.Events;
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
        private IConfig config;
        private ILogger logger = Log.Logger.ForContext<TwitchVotingReceiver>();

        public TwitchVotingReceiver(IConfig config)
        {
            // Validate Config
            if (config.TwitchChannelName == null)
            {
                logger.Fatal("twitch channel name is null, aborting");
                return;
            }
            if (config.TwitchOAuth == null)
            {
                logger.Fatal("twitch oAuth is null, aborting");
                return;
            }
            if (config.TwitchUserName == null)
            {
                logger.Fatal("twitch user name is null, aborting");
                return;
            }

            this.config = config;

            // Connect to twitch
            logger.Information(
                $"trying to connect to channel \"{config.TwitchChannelName}\" with user \"{config.TwitchUserName}\""
            );

            client = new TwitchClient(new WebSocketClient());
            client.Initialize(
                new ConnectionCredentials(config.TwitchUserName, config.TwitchOAuth),
                config.TwitchChannelName
            );

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
        private async void OnDisconnect(object sender, OnDisconnectedArgs e)
        {
            logger.Error("disconnected from the twitch channel, trying to reconnect");
            await Task.Delay(RECONNECT_INTERVAL);
            client.Connect();
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
            logger.Information($"successfully joined twitch channel \"{config.TwitchChannelName}\"");
        }
        private void OnMessageReceived(object sender, OnMessageReceivedArgs e)
        {
            var ChatMessage = e.ChatMessage;

            var evnt = new OnMessageArgs();
            evnt.Message = ChatMessage.Message.Trim();
            evnt.ClientId = ChatMessage.UserId;
            OnMessage.Invoke(this, evnt);
        }
    }
}
