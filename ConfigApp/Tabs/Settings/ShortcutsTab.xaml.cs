using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class ShortcutsTab : UserControl, ITabLifecycle
    {
        public ShortcutsTab()
        {
            InitializeComponent();
        }

        public void OnTabSelected()
        {
        }

        public void OnLoadValues()
        {
            EnableToggleModShortcutCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableToggleModShortcut", true);
            EnableClearActiveEffectsShortcutCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableClearEffectsShortcut", true);
            EnablePauseTimerShortcutCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnablePauseTimerShortcut", false);
            EnableEffectsMenuCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableDebugMenu", false);
            EnableAntiSoftlockShortcutCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableAntiSoftlockShortcut", true);
        }

        public void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EnableToggleModShortcut", EnableToggleModShortcutCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableClearEffectsShortcut", EnableClearActiveEffectsShortcutCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableDebugMenu", EnableEffectsMenuCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnablePauseTimerShortcut", EnablePauseTimerShortcutCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableAntiSoftlockShortcut", EnableAntiSoftlockShortcutCheckBox.IsChecked);
        }
    }
}
