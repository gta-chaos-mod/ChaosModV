using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy
{
    interface IVoteOption
    {
        string Label { get; set; }
        List<string> Matches { get; }
        int Votes { get; set; }
    }
}
