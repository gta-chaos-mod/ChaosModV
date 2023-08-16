namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Config for the default implementation of the overlay server
    /// </summary>
    class OverlayServerConfig
    {
        public bool RetainInitialVotes { get; set; }
        public EVotingMode VotingMode { get; set; }
        public int Port { get; set; }

        public OverlayServerConfig(EVotingMode votingMode, bool retainInitialVotes, int port)
        {
            RetainInitialVotes = retainInitialVotes;
            VotingMode = votingMode;
            Port = port;
        }
    }
}
