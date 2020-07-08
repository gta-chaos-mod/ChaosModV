using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.TwitchConfig
{
    interface ITwtichConfig
    {
        string ChannelName { get; }
        string OAuth { get; }
        string UserName { get; }
        EVotingMode? VotingMode { get; }
    }
}
