using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class SettingsTab : UserControl, ITabLifecycle
    {
        private readonly ITabLifecycle[] m_Tabs;

        public SettingsTab()
        {
            InitializeComponent();
            m_Tabs = [general_tab, modes_tab, shortcuts_tab, colors_tab, sounds_tab];
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
            case "Modes":
                modes_tab.OnTabSelected();
                break;
            case "Shortcuts":
                shortcuts_tab.OnTabSelected();
                break;
            case "Colors":
                colors_tab.OnTabSelected();
                break;
            case "Sounds":
                sounds_tab.OnTabSelected();
                break;
            }
        }

        private void ShowSection(string section)
        {
            general_tab.Visibility = section == "General" ? Visibility.Visible : Visibility.Collapsed;
            modes_tab.Visibility = section == "Modes" ? Visibility.Visible : Visibility.Collapsed;
            shortcuts_tab.Visibility = section == "Shortcuts" ? Visibility.Visible : Visibility.Collapsed;
            colors_tab.Visibility = section == "Colors" ? Visibility.Visible : Visibility.Collapsed;
            sounds_tab.Visibility = section == "Sounds" ? Visibility.Visible : Visibility.Collapsed;
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
