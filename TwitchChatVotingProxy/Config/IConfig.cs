using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.Config
{
    interface IConfig
    {
        string TwitchChannelName { get; }
        string TwitchOAuth { get; }
        string TwitchUserName { get; }
        EVotingMode? VotingMode { get; }
    }
}
