using System;
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

        public override void Initialize(Credentials credentials, string ChannelId)
        {
            client.Initialize(
                new ConnectionCredentials(credentials.UserName, credentials.OAuth),
                ChannelId
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

        public override void SendMessage(string channelId, string message)
        {
            client.SendMessage(channelId, message);
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
