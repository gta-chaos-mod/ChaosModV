using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TwitchChatVotingProxy.Properties;

namespace TwitchChatVotingProxy.OverlayServer
{
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
