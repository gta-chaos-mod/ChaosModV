using System;
using System.Collections.Generic;

namespace VotingProxy.VotingReceiver
{
    /// <summary>
    /// Defines the interface that a voting receiver needs to satisfy
    /// </summary>
    interface IVotingReceiver
    {
        /// <summary>
        /// Events which get invoked when the voting receiver receives a message
        /// </summary>
        event EventHandler<OnMessageArgs> OnMessage;
        /// <summary>
        /// Sends a message to the connected service
        /// </summary>
        /// <param name="message">Message that should be sent</param>
        void SendMessage(List<IVoteOption> options, EVotingMode votingMode);
        void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode);
        void Disconnect();
    }
}
