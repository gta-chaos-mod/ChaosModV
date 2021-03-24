namespace ChaosPipe
{
    public class OnNewVoteArgs
    {
        public string[] VoteOptionNames { get; }

        public OnNewVoteArgs(string[] voteOptionNames)
        {
            VoteOptionNames = voteOptionNames;
        }
    }
}
