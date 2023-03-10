using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.ChaosPipe
{
    class OnSetVotingModeArgs
    {
        public EVotingMode VotingMode { get; }

        public OnSetVotingModeArgs(EVotingMode votingMode)
        {
            VotingMode = votingMode;
        }
    }
}
