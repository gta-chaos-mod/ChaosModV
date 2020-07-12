using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Config for the default implementation of the overlay server
    /// </summary>
    class OverlayServerConfig
    {
        public OverlayServerConfig(EVotingMode votingMode, int? port)
        {
            VotingMode = votingMode;
            Port = port == null ? 9091 : (int)port;
        }

        public EVotingMode VotingMode { get; set; }
        public int Port { get; set; }
    }
}
