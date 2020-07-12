using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    /// <summary>
    /// Defines the interface that a voting receiver needs to satisfy
    /// </summary>
    interface IVotingReceiver
    {
        /// <summary>
        /// Events that gets invoked when the voting receiver receives a message
        /// </summary>
        event EventHandler<OnMessageArgs> OnMessage;
        /// <summary>
        /// Sets a message to the connected service
        /// </summary>
        /// <param name="message">Message that should be sent</param>
        void SendMessage(string message);
    }
}
