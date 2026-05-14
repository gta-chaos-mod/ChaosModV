using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class ModesTab : UserControl, ITabLifecycle
    {
        private enum DispatchModeType
        {
            Time,
            Distance
        }

        public ModesTab()
        {
            InitializeComponent();
            Utils.AttachNumericTextBoxBehavior(EffectDispatchTimerTextBox);
            Utils.AttachNumericTextBoxBehavior(TimedEffectDurationTextBox);
            Utils.AttachNumericTextBoxBehavior(ShortTimedEffectDurationTextBox);
            Utils.AttachNumericTextBoxBehavior(DistanceBasedDispatchDistanceTextBox);
        }

        private void UpdateDispatchModeGridVisibility()
        {
            switch ((DispatchModeType)DispatchModeComboBox.SelectedIndex)
            {
            case DispatchModeType.Time:
                TimePanel.Visibility = Visibility.Visible;
                DistancePanel.Visibility = Visibility.Collapsed;
                break;
            case DispatchModeType.Distance:
                TimePanel.Visibility = Visibility.Collapsed;
                DistancePanel.Visibility = Visibility.Visible;
                break;
            }
        }

        public void OnTabSelected() { }

        public void OnLoadValues()
        {
            DispatchModeComboBox.SelectedIndex = !OptionsManager.ConfigFile.ReadValue("EffectDispatchMode", false, "EnableDistanceBasedEffectDispatch") ? 0 : 1;
            UpdateDispatchModeGridVisibility();
            EffectDispatchTimerTextBox.Text = OptionsManager.ConfigFile.ReadValue("NewEffectSpawnTime", 30).ToString();
            TimedEffectDurationTextBox.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedDur", 90).ToString();
            ShortTimedEffectDurationTextBox.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedShortDur", 30).ToString();
            DistanceBasedDispatchDistanceTextBox.Text = OptionsManager.ConfigFile.ReadValue("DistanceToActivateEffect", 250).ToString();
            DistanceBasedDispatchTypeComboBox.SelectedIndex = OptionsManager.ConfigFile.ReadValue("DistanceType", 0);
            EnableCrossingChallengeCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EnableCrossingChallenge", false);
        }

        public void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValueAsInt("NewEffectSpawnTime", EffectDispatchTimerTextBox.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("EffectTimedDur", TimedEffectDurationTextBox.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("EffectTimedShortDur", ShortTimedEffectDurationTextBox.Text);
            OptionsManager.ConfigFile.WriteValue("EffectDispatchMode", DispatchModeComboBox.SelectedIndex);
            OptionsManager.ConfigFile.WriteValueAsInt("DistanceToActivateEffect", DistanceBasedDispatchDistanceTextBox.Text);
            OptionsManager.ConfigFile.WriteValue("DistanceType", DistanceBasedDispatchTypeComboBox.SelectedIndex);
            OptionsManager.ConfigFile.WriteValue("EnableCrossingChallenge", EnableCrossingChallengeCheckBox.IsChecked);
        }

        private void OnDispatchModeSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateDispatchModeGridVisibility();
        }
    }
}
