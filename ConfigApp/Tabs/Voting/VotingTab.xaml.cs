using ConfigApp.Tabs;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Voting
{
    public sealed partial class VotingTab : UserControl, ITabLifecycle
    {
        private readonly Dictionary<string, ITabLifecycle> m_Tabs;

        public VotingTab()
        {
            InitializeComponent();
            m_Tabs = new Dictionary<string, ITabLifecycle>
            {
                ["General"] = general_tab,
                ["Twitch"] = twitch_tab,
                ["Discord"] = discord_tab
            };
        }

        public void OnTabSelected()
        {
            if (voting_tabs.SelectedItem is TabViewItem selectedTab && selectedTab.Header is string header && m_Tabs.TryGetValue(header, out var tab))
                tab.OnTabSelected();
        }

        public void OnLoadValues()
        {
            foreach (var tab in m_Tabs.Values)
                tab.OnLoadValues();
        }

        public void OnSaveValues()
        {
            foreach (var tab in m_Tabs.Values)
                tab.OnSaveValues();
        }

        private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            OnTabSelected();
        }
    }
}
