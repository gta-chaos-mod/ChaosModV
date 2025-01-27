using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Xceed.Wpf.Toolkit;

namespace ConfigApp.Tabs
{
    public class MiscTab : Tab
    {
        private TextBox? m_EffectDispatchTimer = null;
        private CheckBox? m_DisableDrawTimer = null;
        private TextBox? m_TimedEffectDuration = null;
        private CheckBox? m_DisableDrawEffectText = null;
        private TextBox? m_ShortTimedEffectDuration = null;
        private CheckBox? m_EnableClearActiveEffectsShortcut = null;
        private TextBox? m_RandomSeed = null;
        private CheckBox? m_EnableToggleModShortcut = null;
        private TextBox? m_MaxRunningEffects = null;
        private CheckBox? m_EnableEffectsMenu = null;
        private ColorPicker? m_TimerBarColor = null;
        private CheckBox? m_EnablePauseTimerShortcut = null;
        private ColorPicker? m_EffectTextColor = null;
        private CheckBox? m_EnableAntiSoftlockShortcut = null;
        private ColorPicker? m_EffectTimerBarColor = null;
        private CheckBox? m_EnableEffectGroupWeighting = null;
        private CheckBox? m_DisableModOnStartup = null;
        private CheckBox? m_EnableFailsafe = null;
        private CheckBox? m_EnableModSplashTexts = null;
        private CheckBox? m_EnableDistanceBasedDispatch = null;
        private TextBox? m_DistanceBasedDispatchDistance = null;
        private ComboBox? m_DistanceBasedDispatchType = null;
        private CheckBox? m_EnableCrossingChallenge = null;

        private static ColorPicker GenerateCommonColorPicker(Color defaultColor)
        {
            return new ColorPicker()
            {
                Width = 60f,
                Height = 25f,
                SelectedColor = defaultColor,
                ShowStandardColors = false,
                UsingAlphaChannel = false
            };
        }

        private void SetDistanceDispatchFieldsEnabled(bool state)
        {
            if (m_DistanceBasedDispatchDistance is not null)
                m_DistanceBasedDispatchDistance.IsEnabled = state;
            if (m_DistanceBasedDispatchType is not null)
                m_DistanceBasedDispatchType.IsEnabled = state;
        }

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

            grid.PushRowSpacedPair("New effect timer (in seconds)", m_EffectDispatchTimer = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Don't draw timer bar", m_DisableDrawTimer = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Timed effect duration (in seconds)", m_TimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Don't draw effect text", m_DisableDrawEffectText = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Short timed effect duration (in seconds)", m_ShortTimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Enable clear active effects shortcut (CTRL + -)", m_EnableClearActiveEffectsShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Random Seed (Leave empty for random seed every time)", m_RandomSeed = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Enable toggle mod shortcut (CTRL + L)", m_EnableToggleModShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Max amount of simultaneously running effects", m_MaxRunningEffects = Utils.GenerateCommonNumericOnlyTextBox(2));
            grid.PushRowSpacedPair("Enable effects menu (allows you to choose effects manually, CTRL + ,)", m_EnableEffectsMenu = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Timer bar color", m_TimerBarColor = GenerateCommonColorPicker(Color.FromRgb(0x40, 0x40, 0xFF)));
            grid.PushRowSpacedPair("Enable pause timer shortcut (CTRL + .)", m_EnablePauseTimerShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Effect text color", m_EffectTextColor = GenerateCommonColorPicker(Color.FromRgb(0xFF, 0xFF, 0xFF)));
            grid.PushRowSpacedPair("Enable black screen softlock prevention shortcut (CTRL + SHIFT + K)", m_EnableAntiSoftlockShortcut = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Effect timer bar color", m_EffectTimerBarColor = GenerateCommonColorPicker(Color.FromRgb(0xB4, 0xB4, 0xB4)));
            grid.PushRowSpacedPair("Enable effect group weighting", m_EnableEffectGroupWeighting = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Disable mod on startup by default", m_DisableModOnStartup = Utils.GenerateCommonCheckBox());
            grid.PushRowSpacedPair("Allow prevention of repetitive mission fails (Failsafe)", m_EnableFailsafe = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Show mod splash texts", m_EnableModSplashTexts = Utils.GenerateCommonCheckBox());
            grid.PopRow();

            grid.PopRow();

            m_EnableDistanceBasedDispatch = Utils.GenerateCommonCheckBox();
            m_EnableDistanceBasedDispatch.Click += (sender, eventArgs) =>
            {
                SetDistanceDispatchFieldsEnabled(m_EnableDistanceBasedDispatch.IsChecked.GetValueOrDefault());
            };
            grid.PushRowSpacedPair("Enable distance-based effect dispatch", m_EnableDistanceBasedDispatch);
            grid.PopRow();

            grid.PushRowSpacedPair("Distance to activate effect (in meters)", m_DistanceBasedDispatchDistance = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Type of travel distance", m_DistanceBasedDispatchType = new ComboBox()
            {
                Width = 120f,
                Height = 25f,
                ItemsSource = new string[]
                {
                    "Distance",
                    "Displacement"
                }
            });
            grid.PopRow();

            grid.PushRowSpacedPair("Enable Crossing Challenge™", m_EnableCrossingChallenge = Utils.GenerateCommonCheckBox());

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);

            SetDistanceDispatchFieldsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EffectDispatchTimer is not null)
                m_EffectDispatchTimer.Text = OptionsManager.ConfigFile.ReadValue("NewEffectSpawnTime", "30");
            if (m_DisableDrawTimer is not null)
                m_DisableDrawTimer.IsChecked = OptionsManager.ConfigFile.ReadValueBool("DisableTimerBarDraw", false);
            if (m_TimedEffectDuration is not null)
                m_TimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedDur", "90");
            if (m_DisableDrawEffectText is not null)
                m_DisableDrawEffectText.IsChecked = OptionsManager.ConfigFile.ReadValueBool("DisableEffectTextDraw", false);
            if (m_ShortTimedEffectDuration is not null)
                m_ShortTimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedShortDur", "30");
            if (m_EnableClearActiveEffectsShortcut is not null)
                m_EnableClearActiveEffectsShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableClearEffectsShortcut", true);
            if (m_RandomSeed is not null)
                m_RandomSeed.Text = OptionsManager.ConfigFile.ReadValue("Seed");
            if (m_EnableToggleModShortcut is not null)
                m_EnableToggleModShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableToggleModShortcut", true);
            if (m_MaxRunningEffects is not null)
                m_MaxRunningEffects.Text = OptionsManager.ConfigFile.ReadValue("MaxParallelRunningEffects", "99");
            if (m_EnableEffectsMenu is not null)
                m_EnableEffectsMenu.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableDebugMenu", false);
            if (OptionsManager.ConfigFile.HasKey("EffectTimerColor") && m_TimerBarColor is not null)
                m_TimerBarColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTimerColor"));
            if (m_EnablePauseTimerShortcut is not null)
                m_EnablePauseTimerShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnablePauseTimerShortcut", false);
            if (OptionsManager.ConfigFile.HasKey("EffectTextColor") && m_EffectTextColor is not null)
                m_EffectTextColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTextColor"));
            if (m_EnableAntiSoftlockShortcut is not null)
                m_EnableAntiSoftlockShortcut.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableAntiSoftlockShortcut", true);
            if (OptionsManager.ConfigFile.HasKey("EffectTimedTimerColor") && m_EffectTimerBarColor is not null)
                m_EffectTimerBarColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTimedTimerColor"));
            if (m_EnableEffectGroupWeighting is not null)
                m_EnableEffectGroupWeighting.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableGroupWeightingAdjustments", true);
            if (m_DisableModOnStartup is not null)
                m_DisableModOnStartup.IsChecked = OptionsManager.ConfigFile.ReadValueBool("DisableStartup", false);
            if (m_EnableFailsafe is not null)
                m_EnableFailsafe.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableFailsafe", true);
            if (m_EnableModSplashTexts is not null)
                m_EnableModSplashTexts.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableModSplashTexts", true);
            if (m_EnableDistanceBasedDispatch is not null)
            {
                m_EnableDistanceBasedDispatch.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableDistanceBasedEffectDispatch", false);
                SetDistanceDispatchFieldsEnabled(m_EnableDistanceBasedDispatch.IsChecked.GetValueOrDefault());
            }
            if (m_DistanceBasedDispatchDistance is not null)
                m_DistanceBasedDispatchDistance.Text = OptionsManager.ConfigFile.ReadValue("DistanceToActivateEffect", "250");
            if (m_DistanceBasedDispatchType is not null)
                m_DistanceBasedDispatchType.SelectedIndex = OptionsManager.ConfigFile.ReadValueInt("DistanceType", 0);
            if (m_EnableCrossingChallenge is not null)
                m_EnableCrossingChallenge.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableCrossingChallenge", false);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("NewEffectSpawnTime", m_EffectDispatchTimer?.Text);
            OptionsManager.ConfigFile.WriteValue("DisableTimerBarDraw", m_DisableDrawTimer?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EffectTimedDur", m_TimedEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValue("DisableEffectTextDraw", m_DisableDrawEffectText?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EffectTimedShortDur", m_ShortTimedEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValue("EnableClearEffectsShortcut", m_EnableClearActiveEffectsShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("Seed", m_RandomSeed?.Text);
            OptionsManager.ConfigFile.WriteValue("EnableToggleModShortcut", m_EnableToggleModShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("MaxParallelRunningEffects", m_MaxRunningEffects?.Text);
            OptionsManager.ConfigFile.WriteValue("EnableDebugMenu", m_EnableEffectsMenu?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EffectTimerColor", m_TimerBarColor?.SelectedColor.ToString());
            OptionsManager.ConfigFile.WriteValue("EnablePauseTimerShortcut", m_EnablePauseTimerShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EffectTextColor", m_EffectTextColor?.SelectedColor.ToString());
            OptionsManager.ConfigFile.WriteValue("EnableAntiSoftlockShortcut", m_EnableAntiSoftlockShortcut?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EffectTimedTimerColor", m_EffectTimerBarColor?.SelectedColor.ToString());
            OptionsManager.ConfigFile.WriteValue("EnableGroupWeightingAdjustments", m_EnableEffectGroupWeighting?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("DisableStartup", m_DisableModOnStartup?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableFailsafe", m_EnableFailsafe?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableModSplashTexts", m_EnableModSplashTexts?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("EnableDistanceBasedEffectDispatch", m_EnableDistanceBasedDispatch?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("DistanceToActivateEffect", m_DistanceBasedDispatchDistance?.Text);
            OptionsManager.ConfigFile.WriteValue("DistanceType", m_DistanceBasedDispatchType?.SelectedIndex);
            OptionsManager.ConfigFile.WriteValue("EnableCrossingChallenge", m_EnableCrossingChallenge?.IsChecked);
        }
    }
}
