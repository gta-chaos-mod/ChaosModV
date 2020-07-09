using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    interface IOverlayServer
    {
        void CreateVote(List<IVoteOption> votes);
        void UpdateVotes(List<IVoteOption> votes);
    }
}
