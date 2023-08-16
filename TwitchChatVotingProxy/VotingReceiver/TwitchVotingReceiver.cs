using Serilog;
using Shared;
using TwitchChatVotingProxy.ChaosPipe;
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

        private string m_ChannelName;
        private string m_OAuth;
        private string m_UserName;

        private TwitchClient m_Client;
        private ChaosPipeClient m_ChaosPipe;
        private ILogger m_Logger = Log.Logger.ForContext<TwitchVotingReceiver>();

        private bool m_IsReady = false;

        public TwitchVotingReceiver(OptionsFile config, ChaosPipeClient chaosPipe)
        {
            m_ChannelName = config.ReadValue("TwitchChannelName");
            m_OAuth = config.ReadValue("TwitchChannelOAuth");
            m_UserName = config.ReadValue("TwitchUserName");

            m_ChaosPipe = chaosPipe;
        }

        public async Task<bool> Init()
        {
            if (string.IsNullOrWhiteSpace(m_ChannelName))
            {
                m_Logger.Fatal("Twitch channel is not set!");
                m_ChaosPipe.SendErrorMessage("Twitch channel is not set. Please set one in the config utility.");

                return false;
            }
            if (string.IsNullOrWhiteSpace(m_OAuth))
            {
                m_Logger.Fatal("Twitch OAuth token is not set!");
                m_ChaosPipe.SendErrorMessage("Twitch OAuth token is not set. Please set one in the config utility.");

                return false;
            }
            if (string.IsNullOrWhiteSpace(m_UserName))
            {
                m_Logger.Fatal("Twitch username is not set!");
                m_ChaosPipe.SendErrorMessage("Twitch username is not set. Please set one in the config utility.");

                return false;
            }

            m_Logger.Information($"Trying to connect to twitch channel \"{m_ChannelName}\" with user \"{m_UserName}\"");

            m_Client = new TwitchClient(new WebSocketClient());
            m_Client.Initialize(new ConnectionCredentials(m_UserName, m_OAuth), m_ChannelName);

            m_Client.OnConnected += OnConnected;
            m_Client.OnError += OnError;
            m_Client.OnIncorrectLogin += OnIncorrectLogin;
            m_Client.OnFailureToReceiveJoinConfirmation += OnFailureToReceiveJoinConfirmation;
            m_Client.OnJoinedChannel += OnJoinedChannel;
            m_Client.OnMessageReceived += OnMessageReceived;

            if (!m_Client.Connect())
            {
                m_Logger.Error("Failed to connect to channel");
                return false;
            }

            while (!m_IsReady)
            {
                await Task.Delay(0);
            }

            return true;
        }

        public Task SendMessage(string message)
        {
            try
            {
                foreach (var msg in message.Split("\n"))
                {
                    m_Client.SendMessage(m_ChannelName, msg);
                }
            }
            catch (Exception e)
            {
                m_Logger.Error(e, $"Failed to send message \"{message}\" to channel \"{m_ChannelName}\"");
            }

            return Task.CompletedTask;
        }

        /// <summary>
        /// Called when the twitch client connects (callback)
        /// </summary>
        private void OnConnected(object sender, OnConnectedArgs e)
        {
            m_Logger.Information("Successfully connected to Twitch");
        }
        /// <summary>
        /// Called when the twitch client disconnects (callback)
        /// </summary>
        private async void OnDisconnect(object sender, OnDisconnectedArgs e)
        {
            m_Logger.Error("Disconnected from the twitch channel, trying to reconnect");
            await Task.Delay(RECONNECT_INTERVAL);
            m_Client.Connect();
        }
        /// <summary>
        /// Called when the twitch clients errors (callback)
        /// </summary>
        private void OnError(object sender, OnErrorEventArgs e)
        {
            m_Logger.Error(e.Exception, "Client error, disconnecting");
            m_Client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client has an failed login attempt (callback)
        /// </summary>
        private void OnIncorrectLogin(object sender, OnIncorrectLoginArgs e)
        {
            m_Logger.Error("Invalid Twitch login credentials, check user name and oauth.");
            m_ChaosPipe.SendErrorMessage("Invalid Twitch credentials. Please verify your config.");
            m_Client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client has not received a join confirmation (callback)
        /// </summary>
        private void OnFailureToReceiveJoinConfirmation(object sender, OnFailureToReceiveJoinConfirmationArgs e)
        {
            m_Logger.Error("Invalid Twitch channel, please check specified Twitch channel.");
            m_ChaosPipe.SendErrorMessage("Invalid Twitch channel. Please verify your config.");
            m_Client.Disconnect();
        }
        /// <summary>
        /// Called when the twitch client joins a channel (callback)
        /// </summary>
        private void OnJoinedChannel(object sender, OnJoinedChannelArgs e)
        {
            m_Logger.Information($"Successfully joined Twitch channel \"{m_ChannelName}\"");
            m_IsReady = true;
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
            evnt.Username = chatMessage.Username.ToLower(); // lower case the username to allow case-insensitive comparisons
            OnMessage.Invoke(this, evnt);
        }
    }
}
