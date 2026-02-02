using System;

namespace TwitchChatVotingProxy.VotingReceiver
{
    public class OnMessageArgs : EventArgs
    {
        public string? ClientId { get; set; }
        public string? Username { get; set; }
        public string? Message { get; set; }
    }
}