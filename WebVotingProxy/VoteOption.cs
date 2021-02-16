using System.Collections.Generic;

namespace WebVotingProxy
{
    /// <summary>
    /// Default implementation of the IVoteOption interface
    /// </summary>
    internal class VoteOption : IVoteOption
    {
        public VoteOption(string label, List<string> matches)
        {
            Label = label;
            Matches = matches;
        }

        public string Label { get; set; }
        public List<string> Matches { get; }
        public int Votes { get; set; } = 0;
    }
}
