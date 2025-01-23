namespace TwitchChatVotingProxy.ChaosPipe
{
    class OnSetVotingModeArgs
    {
        public EVotingMode VotingMode { get; }

        public OnSetVotingModeArgs(EVotingMode votingMode)
        {
            VotingMode = votingMode;
        }
    }
}
