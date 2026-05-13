using ConfigApp.Tabs;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class GeneralTab : UserControl, ITabLifecycle
    {
        public GeneralTab()
        {
            InitializeComponent();
            Utils.AttachNumericTextBoxBehavior(MaxRunningEffectsTextBox);
        }

        public void OnTabSelected()
        {
        }

        public void OnLoadValues()
        {
            DisableDrawTimerCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("DisableTimerBarDraw", false);
            DisableDrawEffectTextCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("DisableEffectTextDraw", false);
            RandomSeedTextBox.Text = OptionsManager.ConfigFile.ReadValue<string>("Seed");
            MaxRunningEffectsTextBox.Text = $"{OptionsManager.ConfigFile.ReadValue("MaxParallelRunningEffects", 99)}";
            EnableEffectGroupWeightingCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableGroupWeightingAdjustments", true);
            DisableModOnStartupCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("DisableStartup", false);
            EnableFailsafeCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableFailsafe", true);
            EnableModSplashTextsCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableModSplashTexts", true);
        }

        public void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("DisableTimerBarDraw", DisableDrawTimerCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("DisableEffectTextDraw", DisableDrawEffectTextCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("Seed", RandomSeedTextBox.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MaxParallelRunningEffects", MaxRunningEffectsTextBox.Text);
            OptionsManager.ConfigFile.WriteValue("EnableGroupWeightingAdjustments", EnableEffectGroupWeightingCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("DisableStartup", DisableModOnStartupCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableFailsafe", EnableFailsafeCheckBox.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableModSplashTexts", EnableModSplashTextsCheckBox.IsChecked);
        }
    }
}
