using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy
{
    interface IVoteOption
    {
        /// <summary>
        /// Label (name) of the voting option
        /// </summary>
        string Label { get; set; }
        /// <summary>
        /// What strings should be counted as voting for this option (e.g. 1, 5, etc)
        /// </summary>
        List<string> Matches { get; }
        /// <summary>
        /// How many votes this voting option has
        /// </summary>
        int Votes { get; set; }
    }
}
