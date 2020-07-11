using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Represents how the vote options are being sent to the client after
    /// JSON serializing them.
    /// </summary>
    class OverlayVoteOption
    {
        public OverlayVoteOption(IVoteOption voteOption)
        {
            label = voteOption.Label;
            value = voteOption.Votes;
            matches = voteOption.Matches.ToArray();
        }
        public string label { get; set; }
        public string[] matches { get; set; }
        public int value { get; set; }
    }
}
