using System;
using Serilog;
using System.Collections.Generic;


namespace VotingProxy.VotingDiscordClient
{

    public class DiscordChatClient : DiscordClient
    {
        public DiscordBot client;
        private DiscordCredentials creds;
        private ILogger logger = Log.Logger.ForContext<DiscordChatClient>();

        public DiscordChatClient(DiscordCredentials credentials)
        {
            client = new DiscordBot(credentials);
            creds = credentials;
        }

        public override void Connect()
        {
            logger.Information("Connect method called");
            client.StartBot();
        }

        public override void Disconnect()
        {
            logger.Information("Disconnect method called");
            client.StopBot();
        }

        public override void SendMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            logger.Information("SendMessage method called");
            client.SendVoteMessage(options, votingMode);
        }

        public sealed override event EventHandler OnDiscordConnected
        {
            add
            {
                logger.Information("Connect handler added");
                client.OnBotConnected += value;
            }
            remove 
            {
                logger.Information("Connect handler removed");
                client.OnBotConnected -= value;
            }
        }

        public sealed override event EventHandler<OnDiscordMessageReceivedArgs> OnDiscordMessageReceived
        {
            add
            {
                logger.Information("Message handler added");
                client.OnVoteReceived += (object sender, BotVoteEventArgs e) =>
                {
                    ChatMessage e_ChatMessage = new ChatMessage()
                    {
                        Message = e.Vote.ToString(),
                        UserId = e.UserId.ToString()
                    };
                    OnDiscordMessageReceivedArgs evt = new OnDiscordMessageReceivedArgs()
                    { 
                        ChatMessage = e_ChatMessage
                    };
                    value(sender, evt);
                };
            }
            remove
            {
                logger.Information("Message handler removed");
                client.OnVoteReceived -= (object sender, BotVoteEventArgs e) =>
                {
                    ChatMessage e_ChatMessage = new ChatMessage()
                    {
                        Message = e.Vote.ToString(),
                        UserId = e.UserId.ToString()
                    };
                    OnDiscordMessageReceivedArgs evt = new OnDiscordMessageReceivedArgs()
                    {
                        ChatMessage = e_ChatMessage
                    };
                    value(sender, evt);
                };
            }
        }
    }
}
