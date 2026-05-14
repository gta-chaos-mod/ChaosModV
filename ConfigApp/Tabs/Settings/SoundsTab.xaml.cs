using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class SoundsTab : UserControl, ITabLifecycle
    {
        public SoundsTab()
        {
            InitializeComponent();
        }

        public void OnTabSelected() { }

        public void OnLoadValues()
        {
            UseMciCheckBox.IsChecked = OptionsManager.ConfigFile.ReadValue("EffectSoundUseMCI", false);
        }

        public void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EffectSoundUseMCI", UseMciCheckBox.IsChecked);
        }
    }
}
