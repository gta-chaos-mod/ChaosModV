using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    interface IOverlayServer
    {
        void EndVoting();
        void NewVoting(List<IVoteOption> votes);
        void UpdateVoting(List<IVoteOption> votes);
    }
}
