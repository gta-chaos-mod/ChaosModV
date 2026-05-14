using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class SettingsTab : UserControl, ITabLifecycle
    {
        private readonly Dictionary<string, ITabLifecycle> m_Tabs;

        public SettingsTab()
        {
            InitializeComponent();
            m_Tabs = new Dictionary<string, ITabLifecycle>
            {
                ["General"] = general_tab,
                ["Modes"] = modes_tab,
                ["Shortcuts"] = shortcuts_tab,
                ["Colors"] = colors_tab,
                ["Sounds"] = sounds_tab
            };
        }

        public void OnTabSelected()
        {
            if (settings_tabs.SelectedItem is TabViewItem selectedTab && selectedTab.Header is string header && m_Tabs.TryGetValue(header, out var tab))
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
