namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Message which is being sent 
    /// </summary>
    class OverlayMessage
    {
        public bool? RetainInitialVotes { get; set; } = null;
        /// <summary>
        /// request type
        /// </summary>
        public string? Request { get; set; } = null;
        /// <summary>
        /// total vote count
        /// </summary>
        public int? TotalVotes { get; set; } = null;
        /// <summary>
        /// what voting mode should be used, this results in display changes
        /// </summary>
        public string? VotingMode { get; set; } = null;
        /// <summary>
        /// Voting options them self
        /// </summary>
        public OverlayVoteOption[]? VoteOptions { get; set; } = null;
    }
}
