using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    /// <summary>
    /// Event which should be dispatched when the voting receiver receives
    /// a message.
    /// </summary>
    class OnMessageArgs
    {
        public string ClientId { get; set; }
        public string Message { get; set; }
    }
}
