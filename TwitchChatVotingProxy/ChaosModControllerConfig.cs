namespace TwitchChatVotingProxy
{
    class ChaosModControllerConfig
    {
        public EVotingMode VotingMode { get; set; }
        public EOverlayMode OverlayMode { get; set; }
        public bool RetainInitialVotes { get; set; }
        public string[] PermittedUsernames { get; set; }
        public string VoteablePrefix { get; set; }
    }
}
