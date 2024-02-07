namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Represents how the vote options are being sent to the client after
    /// JSON serializing them with properties converted to lower case after the fact.
    /// </summary>
    class OverlayVoteOption
    {
        public string Label { get; set; }
        public string[] Matches { get; set; }
        public int Value { get; set; }

        public OverlayVoteOption(IVoteOption voteOption)
        {
            Label = voteOption.Label;
            Value = voteOption.Votes;
            Matches = voteOption.Matches.ToArray();
        }
    }
}
