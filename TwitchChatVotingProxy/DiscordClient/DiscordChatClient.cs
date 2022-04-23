using System;
using Serilog;
using System.Collections.Generic;


namespace VotingProxy.VotingChatClient
{

    public class DiscordChatClient : ChatClient
    {
        public DiscordBot client;
        private Credentials creds;
        private ILogger logger = Log.Logger.ForContext<DiscordChatClient>();

        public DiscordChatClient(){}

        public override void Initialize(Credentials credentials)
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

        public override void SendMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId)
        {
            client.SendVoteMessage(options, votingMode);
        }

        public override void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId)
        {
            client.UpdateVoteMessage(options, votingMode);
        }

        public sealed override event EventHandler OnConnected
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

        public sealed override event EventHandler<OnMessageReceivedArgs> OnMessageReceived
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
                    OnMessageReceivedArgs evt = new OnMessageReceivedArgs()
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
                    OnMessageReceivedArgs evt = new OnMessageReceivedArgs()
                    {
                        ChatMessage = e_ChatMessage
                    };
                    value(sender, evt);
                };
            }
        }

        public sealed override event EventHandler OnIncorrectLogin{add{}remove{}}

        public sealed override event EventHandler OnJoinedChannel{add{}remove{}}

        public sealed override event EventHandler<OnErrorEventArgs> OnError{add{}remove{}}
    }
}
