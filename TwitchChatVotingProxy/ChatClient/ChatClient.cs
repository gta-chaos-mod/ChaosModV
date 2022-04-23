using System;
using System.Collections.Generic;

namespace VotingProxy.VotingChatClient
{
    public abstract class ChatClient
    {
        public virtual event EventHandler OnConnected;
        public virtual event EventHandler OnDisconnect;
        public virtual event EventHandler<OnErrorEventArgs> OnError;
        public virtual event EventHandler OnIncorrectLogin;
        public virtual event EventHandler OnJoinedChannel;
        public virtual event EventHandler<OnMessageReceivedArgs> OnMessageReceived;

        public abstract void SendMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId);
        public abstract void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode, string channelId);
        public abstract void Connect();
        public abstract void Disconnect();
        public abstract void Initialize(Credentials credentials);
    }

    public class Credentials
    {
        public string UserId;
        public string OAuth;
        public string ChannelId;
        public Credentials(string channelId, string userName, string oAuth)
        {
            OAuth = oAuth;
            UserId = userName;
            ChannelId = channelId;
        }
    }

    public class OnErrorEventArgs : EventArgs
    {
        public Exception Exception;
    }

    public class OnMessageReceivedArgs : EventArgs
    {
        public ChatMessage ChatMessage;
    }

    public class ChatMessage
    {
        public string Message { get; set; }
        public string UserId { get; set; }
    }
}
