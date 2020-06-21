using System;

namespace TwitchChatVotingProxy
{
    class VoteUser
    {
        public string UserId;
        public int VotedForIndex = -1;

        public VoteUser(string userId)
        {
            UserId = userId;
        }
    }
}
