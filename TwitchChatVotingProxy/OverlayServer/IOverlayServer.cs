using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.OverlayServer
{
    interface IOverlayServer
    {
        /// <summary>
        /// Tells the overlay server that the voting has ended
        /// </summary>
        void EndVoting();
        /// <summary>
        /// Tells the overlay server about a new vote
        /// </summary>
        /// <param name="voteOptions">The new voting options</param>
        void NewVoting(List<IVoteOption> voteOptions);
        void UpdateVoting(List<IVoteOption> votes);
    }
}
