using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    class OnMessageArgs
    {
        public string ClientId { get; set; }
        public string Message { get; set; }
    }
}
