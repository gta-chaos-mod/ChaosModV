namespace VotingProxy.VotingReceiver
{
    /// <summary>
    /// Event which should be dispatched when the voting receiver receives
    /// a message.
    /// </summary>
    class OnDiscordMessageArgs
    {
        public string ClientId { get; set; }
        public string Message { get; set; }
    }
}
