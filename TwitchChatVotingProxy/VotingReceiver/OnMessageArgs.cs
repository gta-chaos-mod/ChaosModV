﻿namespace TwitchChatVotingProxy.VotingReceiver
{
    /// <summary>
    /// Event which should be dispatched when the voting receiver receives
    /// a message.
    /// </summary>
    class OnMessageArgs
    {
        public string? ClientId { get; set; } = null;
        public string? Message { get; set; } = null;
        public string? Username { get; set; } = null;
    }
}
