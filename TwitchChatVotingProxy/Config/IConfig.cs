using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.Config
{
    interface IConfig
    {
        int? OverlayServerPort { get; set; }
        string TwitchChannelName { get; set; }
        string TwitchOAuth { get; set; }
        string TwitchUserName { get; set; }
        EVotingMode? VotingMode { get; set; }
    }
}
