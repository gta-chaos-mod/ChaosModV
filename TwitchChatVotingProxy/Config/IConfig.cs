namespace TwitchChatVotingProxy.Config
{
    interface IConfig
    {
        EOverlayMode? OverlayMode { get; set; }
        EVotingMode? VotingMode { get; set; }
        int? OverlayServerPort { get; set; }
        bool RetainInitalVotes { get; set; }
        string TwitchChannelName { get; set; }
        string TwitchOAuth { get; set; }
        string TwitchUserName { get; set; }
        public string[] PermittedTwitchUsernames { get; set; }
        public string VoteablePrefix { get; set; }
    }
}
