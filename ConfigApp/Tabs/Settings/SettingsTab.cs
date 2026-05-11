using ConfigApp.Tabs;

namespace ConfigApp.Tabs.Settings
{
    public class SettingsTab : CompositeTab
    {
        private readonly Dictionary<string, Tab> m_Tabs = new()
        {
            { "General", new GeneralTab() },
            { "Modes", new ModesTab() },
            { "Shortcuts", new ShortcutsTab() },
            { "Colors", new ColorsTab() },
            { "Sounds", new SoundsTab() }
        };

        protected override IReadOnlyDictionary<string, Tab> ChildTabs => m_Tabs;
    }
}
