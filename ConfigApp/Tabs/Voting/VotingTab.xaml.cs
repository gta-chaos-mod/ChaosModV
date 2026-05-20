using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Voting
{
    public sealed partial class VotingTab : UserControl, ITabLifecycle
    {
        private readonly ITabLifecycle[] m_Tabs;

        public VotingTab()
        {
            InitializeComponent();
            m_Tabs = [general_tab, twitch_tab, discord_tab];
        }

        public void OnTabSelected()
        {
            ShowSection("General");
            general_tab.OnTabSelected();
        }

        public void SelectSubSection(string section)
        {
            ShowSection(section);

            switch (section)
            {
            case "General":
                general_tab.OnTabSelected();
                break;
            case "Twitch":
                twitch_tab.OnTabSelected();
                break;
            case "Discord":
                discord_tab.OnTabSelected();
                break;
            }
        }

        private void ShowSection(string section)
        {
            general_tab.Visibility = section == "General" ? Visibility.Visible : Visibility.Collapsed;
            twitch_tab.Visibility = section == "Twitch" ? Visibility.Visible : Visibility.Collapsed;
            discord_tab.Visibility = section == "Discord" ? Visibility.Visible : Visibility.Collapsed;
        }

        public void OnLoadValues()
        {
            foreach (var tab in m_Tabs)
                tab.OnLoadValues();
        }

        public void OnSaveValues()
        {
            foreach (var tab in m_Tabs)
                tab.OnSaveValues();
        }
    }
}
