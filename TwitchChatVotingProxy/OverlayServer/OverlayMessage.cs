using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    class OverlayMessage
    {
        public string request { get; set; }
        public int totalVotes { get; set; }
        public string votingMode { get; set; }

        public OverlayVoteOption[] voteOptions { get; set; }
    }
}
