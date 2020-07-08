using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    interface IOverlayServer
    {
        void createVote(List<IVoteOption> votes);
        void updateVotes(List<IVoteOption> votes);
    }
}
