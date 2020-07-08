using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.ChaosPipe
{
    interface IChaosPipeClient
    {
        event EventHandler<OnGetVoteResultArgs> OnGetVoteResult;
        event EventHandler<OnNewVoteArgs> OnNewVote;
    }
}
