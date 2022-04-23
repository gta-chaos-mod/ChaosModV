using System;
using System.Collections.Generic;

namespace VotingProxy.VotingReceiver
{
    /// <summary>
    /// Defines the interface that a voting receiver needs to satisfy
    /// </summary>
    interface IVotingReceiverDiscord
    {
        /// <summary>
        /// Events which get invoked when the voting receiver receives a message
        /// </summary>
        event EventHandler<OnDiscordMessageArgs> OnDiscordMessage;

        void SendMessage(List<IVoteOption> options, EVotingMode votingMode);
        void UpdateMessage(List<IVoteOption> options, EVotingMode votingMode);
    }
}
