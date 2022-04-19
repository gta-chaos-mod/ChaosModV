using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VotingProxy.Config
{
    interface IConfig
    {
        EOverlayMode? OverlayMode { get; set; }
        EVotingMode? VotingMode { get; set; }
        int? OverlayServerPort { get; set; }
        bool RetainInitalVotes { get; set; }
        string ChannelId { get; set; }
        string OAuth { get; set; }
        string UserName { get; set; }
    }
}
