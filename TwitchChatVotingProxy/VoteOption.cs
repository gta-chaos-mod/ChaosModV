using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy
{
    /// <summary>
    /// Default implementation of the IVoteOption interface
    /// </summary>
    class VoteOption : IVoteOption
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
