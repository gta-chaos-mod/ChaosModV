namespace WebVotingProxy.ChaosPipe
{
    internal class OnGetVoteResultArgs
    {
        /// <summary>
        /// The chosen option that should be sent to the chaos mod pipe
        /// </summary>
        public int? ChosenOption { get; set; } = null;
    }
}
