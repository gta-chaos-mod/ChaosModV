using ConfigApp.Tabs;

namespace ConfigApp.Tabs.Voting
{
    public class VotingTab : CompositeTab
    {
        private readonly Dictionary<string, Tab> m_Tabs = new()
        {
            { "General", new GeneralTab() },
            { "Twitch", new TwitchTab() },
            { "Discord", new DiscordTab() }
        };

        protected override IReadOnlyDictionary<string, Tab> ChildTabs => m_Tabs;
    }
}
