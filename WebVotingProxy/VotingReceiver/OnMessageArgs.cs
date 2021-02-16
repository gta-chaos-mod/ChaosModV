namespace WebVotingProxy.VotingReceiver
{
    /// <summary>
    /// Event which should be dispatched when the voting receiver receives
    /// a message.
    /// </summary>
    internal class OnMessageArgs
    {
        public string ClientId { get; set; }
        public int Idx { get; set; }
    }
}
