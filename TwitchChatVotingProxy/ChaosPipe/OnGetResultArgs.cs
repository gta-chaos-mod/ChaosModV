using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.ChaosPipe
{
    class OnGetVoteResultArgs
    {
        /// <summary>
        /// The chosen option that should be sent to the chaos mod pipe
        /// </summary>
        public int? ChosenOption { get; set; } = null;
    }
}
