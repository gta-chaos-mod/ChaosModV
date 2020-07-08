using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    class OnMessageArgs
    {
        public OnMessageArgs(string message)
        {
            Message = message;
        }
        public string Message { get; }
    }
}
