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
            client.StartBot();
        }

        public override void Disconnect()
        {
            client.StopBot();
        }

        public override void SendMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            client.SendVoteMessage(options, votingMode);
        }

        public override void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            client.UpdateVoteMessage(options, votingMode);
        }

        public sealed override event EventHandler OnDiscordConnected
        {
            add
            {
                client.OnBotConnected += value;
            }
            remove 
            {
                client.OnBotConnected -= value;
            }
        }

        public sealed override event EventHandler<OnDiscordMessageReceivedArgs> OnDiscordMessageReceived
        {
            add
            {
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
