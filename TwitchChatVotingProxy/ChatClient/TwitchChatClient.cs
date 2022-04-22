using System;
using System.Collections.Generic;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;

namespace VotingProxy.VotingChatClient
{

    class TwitchChatClient : ChatClient
    {
        private TwitchClient client;

        public TwitchChatClient()
        {
            client = new TwitchClient(new WebSocketClient());
        }

        public override void Initialize(Credentials credentials)
        {
            client.Initialize(
                new ConnectionCredentials(credentials.UserId, credentials.OAuth),
                credentials.ChannelId
            );
        }

        public override void Connect()
        {
            client.Connect();
        }

        public override void Disconnect()
        {
            client.Disconnect();
        }

        public override void SendMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId)
        {
            client.SendMessage("Time for a new effect! Vote between:", channelId);
            foreach (IVoteOption voteOption in options)
            {
                string msg = string.Empty;

                bool firstIndex = true;
                foreach (string match in voteOption.Matches)
                {
                    msg += firstIndex ? $"{match} " : $" / {match}";

                    firstIndex = true;
                }

                msg += $": {voteOption.Label}\n";

                client.SendMessage(msg, channelId);
            }

            if (votingMode == EVotingMode.PERCENTAGE)
            {
                client.SendMessage("Votes will affect the chance for one of the effects to occur.", channelId);
            }
        }

        public override void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId)
        {

        }

        public sealed override event EventHandler OnConnected
        {
            add
            {
                client.OnConnected += new EventHandler<OnConnectedArgs>(value);
            }
            remove {
                client.OnConnected -= new EventHandler<OnConnectedArgs>(value);
            }
        }

        public sealed override event EventHandler<OnErrorEventArgs> OnError
        {
            add
            {
                client.OnError += (object sender, TwitchLib.Communication.Events.OnErrorEventArgs e) =>
                {
                    OnErrorEventArgs evt = new OnErrorEventArgs();
                    evt.Exception = e.Exception;

                    value(sender, evt);
                };
            }
            remove
            {
                client.OnError -= (object sender, TwitchLib.Communication.Events.OnErrorEventArgs e) =>
                {
                    OnErrorEventArgs evt = new OnErrorEventArgs();
                    evt.Exception = e.Exception;

                    value(sender, evt);
                };
            }
        }

        public sealed override event EventHandler<OnMessageReceivedArgs> OnMessageReceived
        {
            add
            {
                client.OnMessageReceived += (object sender, TwitchLib.Client.Events.OnMessageReceivedArgs e) =>
                {
                    ChatMessage e_ChatMessage = new ChatMessage()
                    {
                        Message = e.ChatMessage.Message,
                        UserId = e.ChatMessage.UserId
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
                client.OnMessageReceived -= (object sender, TwitchLib.Client.Events.OnMessageReceivedArgs e) =>
                {
                    ChatMessage e_ChatMessage = new ChatMessage()
                    {
                        Message = e.ChatMessage.Message,
                        UserId = e.ChatMessage.UserId
                    };
                    OnMessageReceivedArgs evt = new OnMessageReceivedArgs()
                    {

                        ChatMessage = e_ChatMessage
                    };

                    value(sender, evt);
                };
            }
        }

        public sealed override event EventHandler OnIncorrectLogin
        {
            add
            {
                client.OnIncorrectLogin += new EventHandler<OnIncorrectLoginArgs>(value);
            }
            remove
            {
                client.OnIncorrectLogin -= new EventHandler<OnIncorrectLoginArgs>(value);
            }
        }

        public sealed override event EventHandler OnJoinedChannel
        {
            add
            {
                client.OnJoinedChannel += new EventHandler<OnJoinedChannelArgs>(value);
            }
            remove
            {
                client.OnJoinedChannel -= new EventHandler<OnJoinedChannelArgs>(value);
            }
        }
    }
}
