using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class SettingsTab : Tab
    {
        private readonly Dictionary<string, Tab> m_Tabs = new()
        {
            { "General", new GeneralTab() },
            { "Modes", new ModesTab() },
            { "Shortcuts", new ShortcutsTab() },
            { "Colors", new ColorsTab() },
            { "Sounds", new SoundsTab() }
        };

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var tabControl = new TabView();

            foreach (var tab in m_Tabs)
            {
                var tabItem = new TabViewItem()
                {
                    Header = tab.Key,
                };

                var grid = new Grid();

                tab.Value.Init(grid);

                tabItem.Content = grid;

                tabControl.TabItems.Add(tabItem);
            }

            PushRowElement(tabControl);
            PopRow();
        }

        public override void OnLoadValues()
        {
            foreach (var tab in m_Tabs)
                tab.Value.OnLoadValues();
        }

        public override void OnSaveValues()
        {
            foreach (var tab in m_Tabs)
                tab.Value.OnSaveValues();
        }
    }
}
