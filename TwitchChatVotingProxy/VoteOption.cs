using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.Properties
{
    /// <summary>
    /// Default implementation for a vote option
    /// </summary>
    class VoteOption : IVoteOption
    {
        public VoteOption(string label, List<string> matches)
        {
            this.Label = label;
            this.Matches = matches;
        }

        public string Label { get; set; }

        public List<string> Matches { get; }

        public int Votes { get; set; }
    }
}
