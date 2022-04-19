using System;

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

        public abstract void SendMessage(string channelId, string message);
        public abstract void Connect();
        public abstract void Disconnect();
        public abstract void Initialize(Credentials credentials, string ChannelId);
    }

    public class Credentials
    {
        public string UserName;
        public string OAuth;
        public Credentials(string userName, string oAuth)
        {
            OAuth = oAuth;
            UserName = userName;
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
