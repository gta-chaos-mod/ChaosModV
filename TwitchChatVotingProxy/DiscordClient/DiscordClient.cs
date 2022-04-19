using System;
using VotingProxy.VotingReceiver;
using System.Collections.Generic;

namespace VotingProxy.VotingDiscordClient
{
    public abstract class DiscordClient
    {
        public virtual event EventHandler OnDiscordConnected;
        public virtual event EventHandler OnDisconnect;
        public virtual event EventHandler OnDiscordIncorrectLogin;
        public virtual event EventHandler<OnDiscordMessageReceivedArgs> OnDiscordMessageReceived;

        public abstract void SendMessage(List<IVoteOption> options, EVotingMode votingMode);
        public abstract void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode);
        public abstract void Connect();
        public abstract void Disconnect();
    }

    public class DiscordCredentials
    {
        public string ChannelId;
        public string GuildID;
        public string OAuth;
        public DiscordCredentials(string channelId, string guildID, string oAuth)
        {
            OAuth = oAuth;
            ChannelId = channelId;
            GuildID = guildID;
        }
    }

    public class OnDiscordMessageReceivedArgs : EventArgs
    {
        public ChatMessage ChatMessage;
    }

    public class ChatMessage
    {
        public string Message { get; set; }
        public string UserId { get; set; }
    }
}
