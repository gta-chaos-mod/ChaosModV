using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class ShortcutsTab : Tab
    {
        private CheckBox? m_EnableToggleModShortcut = null;
        private CheckBox? m_EnableClearActiveEffectsShortcut = null;
        private CheckBox? m_EnablePauseTimerShortcut = null;
        private CheckBox? m_EnableEffectsMenu = null;
        private CheckBox? m_EnableAntiSoftlockShortcut = null;

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var scrollViewer = new ScrollViewer();

            var grid = new ChaosGrid();
            grid.PushNewColumn(new GridLength(310f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength(100f));
            grid.PushNewColumn(new GridLength(450f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength());

            grid.PushRowSpacedPair("Enable toggle mod shortcut (CTRL + L)", m_EnableToggleModShortcut = Utils.GenerateCommonCheckBox());
            grid.PushRowSpacedPair("Enable clear active effects shortcut (CTRL + -)", m_EnableClearActiveEffectsShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Enable pause timer shortcut (CTRL + .)", m_EnablePauseTimerShortcut = Utils.GenerateCommonCheckBox());
            grid.PushRowSpacedPair("Enable effects menu (allows you to choose effects manually, CTRL + ,)", m_EnableEffectsMenu = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Enable black screen softlock prevention shortcut (CTRL + SHIFT + K)", m_EnableAntiSoftlockShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (m_EnableToggleModShortcut is not null)
                m_EnableToggleModShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableToggleModShortcut", true);
            if (m_EnableClearActiveEffectsShortcut is not null)
                m_EnableClearActiveEffectsShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableClearEffectsShortcut", true);
            if (m_EnablePauseTimerShortcut is not null)
                m_EnablePauseTimerShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnablePauseTimerShortcut", false);
            if (m_EnableEffectsMenu is not null)
                m_EnableEffectsMenu.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableDebugMenu", false);
            if (m_EnableAntiSoftlockShortcut is not null)
                m_EnableAntiSoftlockShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableAntiSoftlockShortcut", true);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EnableToggleModShortcut", m_EnableToggleModShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableClearEffectsShortcut", m_EnableClearActiveEffectsShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableDebugMenu", m_EnableEffectsMenu?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnablePauseTimerShortcut", m_EnablePauseTimerShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableAntiSoftlockShortcut", m_EnableAntiSoftlockShortcut?.IsChecked);
        }
    }
}
