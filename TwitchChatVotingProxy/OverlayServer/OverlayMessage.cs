namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Message which is being sent 
    /// </summary>
    class OverlayMessage
    {
        public bool retainInitialVotes { get; set; }
        /// <summary>
        /// request type
        /// </summary>
        public string request { get; set; }
        /// <summary>
        /// total vote count
        /// </summary>
        public int totalVotes { get; set; }
        /// <summary>
        /// what voting mode should be used, this results in display changes
        /// </summary>
        public string votingMode { get; set; }
        /// <summary>
        /// Voting options them self
        /// </summary>
        public OverlayVoteOption[] voteOptions { get; set; }
    }
}
