using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    interface IVotingReceiver
    {
        event EventHandler<OnMessageArgs> OnMessage;
    }
}
