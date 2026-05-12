using ConfigApp.Infrastructure;
using Microsoft.UI.Input;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Windows.System;
using Windows.UI.Core;

using static ConfigApp.Effects;

namespace ConfigApp
{
    public partial class EffectConfig : ContentDialog
    {
        private static readonly string[] s_TimedTypeOptions =
        {
            "Normal",
            "Short",
            "Permanent"
        };

        private static readonly string[] s_WeightMultiplierOptions =
        {
            "Default", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
        };

        private static readonly string[] s_ExcludeVotingOptions =
        {
            "Default", "Disabled", "Enabled"
        };

        private readonly EffectData m_EffectData;
        private readonly bool m_IsTimedEffect;
        private bool m_IsSaved;
        private int m_EffectShortcut; // Win32Key + 2^10 (if CTRL) + 2^9 (if Shift) + 2^8 (if Alt)

        public bool IsSaved => m_IsSaved;

        public EffectConfig(string? effectId, EffectData? effectData, EffectInfo effectInfo)
        {
            InitializeComponent();
            if (App.MainWindow?.Content is FrameworkElement rootElement)
                XamlRoot = rootElement.XamlRoot;

            Utils.AttachNumericTextBoxBehavior(effectconf_timer_time);

            m_EffectData = effectData ?? new EffectData();
            m_IsTimedEffect = effectInfo.IsTimed;

            Title = effectInfo.Name;

            InitializeTimedOptions(effectInfo);
            InitializeCommonOptions(effectId, effectInfo);
            InitializeShortcutOptions(effectId);
            InitializeExistingShortcut();

            CheckEnableConfigurables();
        }

        private void InitializeTimedOptions(EffectInfo effectInfo)
        {
            var defaultTimedType = effectInfo.IsTimed
                ? (effectInfo.IsShort ? EffectTimedType.Short : EffectTimedType.Normal)
                : EffectTimedType.NotTimed;

            effectconf_timer_type.ItemsSource = s_TimedTypeOptions;
            effectconf_timer_type.SelectedIndex = m_EffectData.TimedType.GetValueOrDefault(defaultTimedType) switch
            {
                EffectTimedType.Normal => 0,
                EffectTimedType.Short => 1,
                EffectTimedType.Permanent => 2,
                _ => 0
            };

            effectconf_timer_type_enable.IsChecked = m_EffectData.TimedType.HasValue
                && m_EffectData.TimedType != defaultTimedType
                && m_EffectData.TimedType != EffectTimedType.Custom;

            if (m_EffectData.CustomTime > 0)
            {
                effectconf_timer_time_enable.IsChecked = true;
                effectconf_timer_time.Text = $"{m_EffectData.CustomTime}";
            }

            effectconf_effect_weight_mult.ItemsSource = s_WeightMultiplierOptions;
            effectconf_effect_weight_mult.SelectedIndex = m_EffectData.WeightMult.GetValueOrDefault(0);

            effectconf_exclude_voting_state.ItemsSource = s_ExcludeVotingOptions;
            effectconf_exclude_voting_state.SelectedIndex = m_EffectData.ExcludedFromVoting.HasValue
                ? (m_EffectData.ExcludedFromVoting.Value ? 2 : 1)
                : 0;

            if (!m_IsTimedEffect)
            {
                effectconf_timer_type_enable.IsEnabled = false;
                effectconf_timer_time_enable.IsEnabled = false;
            }
        }

        private void InitializeCommonOptions(string? effectId, EffectInfo effectInfo)
        {
            effectconf_effect_custom_name.Text = m_EffectData.CustomName;
            effectconf_effect_custom_name.TextChanged += OnCustomEffectNameTextFieldTextChanged;

            if (!string.IsNullOrWhiteSpace(effectId))
            {
                effectconf_mp3_label.Text = $@"
                    Sound to play when this effect gets activated: chaosmod/sounds/{effectId}.mp3
                    Or create the following folder and drop mp3 files in there to play a random one: chaosmod/sounds/{effectId}
                ";
            }

            if (effectInfo.EffectCategory == EffectCategory.Meta)
            {
                effectconf_exclude_voting_enable_title.Visibility = Visibility.Collapsed;
                effectconf_exclude_voting_state.Visibility = Visibility.Collapsed;
                effectconf_exclude_voting_state.SelectedIndex = 0;
            }
        }

        private void InitializeShortcutOptions(string? effectId)
        {
            if (effectId is not null)
            {
                effectconf_effect_shortcut_enable.IsChecked = true;
                effectconf_effect_shortcut_enable.Visibility = Visibility.Collapsed;
                SetShortcutInputEnabled(true);
                return;
            }

            SetShortcutInputEnabled(false);
        }

        private void InitializeExistingShortcut()
        {
            if (!m_EffectData.ShortcutKeycode.HasValue)
                return;

            var savedWin32Key = m_EffectData.ShortcutKeycode.Value;
            SetShortcutInputEnabled(true);

            if (savedWin32Key <= 0)
            {
                effectconf_effect_shortcut_input.Text = "None";
                return;
            }

            var key = (VirtualKey)(savedWin32Key % 256);
            var ctrl = (savedWin32Key & (1 << 10)) != 0;
            var shift = (savedWin32Key & (1 << 9)) != 0;
            var alt = (savedWin32Key & (1 << 8)) != 0;

            SetEffectShortcut(key, ctrl, shift, alt);

            if (effectconf_effect_shortcut_enable.Visibility == Visibility.Visible)
                effectconf_effect_shortcut_enable.IsChecked = true;
        }

        private void OnCustomEffectNameTextFieldTextChanged(object sender, TextChangedEventArgs e)
        {
            var sanitized = effectconf_effect_custom_name.Text.Replace("\"", string.Empty);
            if (!string.Equals(effectconf_effect_custom_name.Text, sanitized, StringComparison.Ordinal))
                effectconf_effect_custom_name.Text = sanitized;
        }

        // PreviewKeyDown so that text editing shortcuts such as "CTRL+C" are possible
        private void EffectShortcutTextFieldKeyDown(object sender, KeyRoutedEventArgs e)
        {
            e.Handled = true;

            var key = e.Key;
            var ctrl = InputKeyboardSource.GetKeyStateForCurrentThread(VirtualKey.Control).HasFlag(CoreVirtualKeyStates.Down);
            var shift = InputKeyboardSource.GetKeyStateForCurrentThread(VirtualKey.Shift).HasFlag(CoreVirtualKeyStates.Down);
            var alt = InputKeyboardSource.GetKeyStateForCurrentThread(VirtualKey.Menu).HasFlag(CoreVirtualKeyStates.Down);

            if (key is VirtualKey.Escape or VirtualKey.Back)
            {
                effectconf_effect_shortcut_input.Text = "None";
                m_EffectShortcut = 0;
                return;
            }

            if (key is VirtualKey.Control or VirtualKey.Shift or VirtualKey.Menu or VirtualKey.LeftWindows or VirtualKey.RightWindows)
                return;

            SetEffectShortcut(key, ctrl, shift, alt);
        }

        private void SetEffectShortcut(VirtualKey key, bool ctrl, bool shift, bool alt)
        {
            m_EffectShortcut = BuildShortcutKeycode(key, ctrl, shift, alt);
            effectconf_effect_shortcut_input.Text = Utils.FormatShortcutDisplay(key, ctrl, shift, alt);
        }

        private static int BuildShortcutKeycode(VirtualKey key, bool ctrl, bool shift, bool alt)
        {
            var shortcut = (int)key;

            if (ctrl)
                shortcut |= 1 << 10;
            if (shift)
                shortcut |= 1 << 9;
            if (alt)
                shortcut |= 1 << 8;

            return shortcut;
        }

        private void SetShortcutInputEnabled(bool enabled)
        {
            effectconf_effect_shortcut_input.IsEnabled = enabled;
        }

        private void CheckEnableConfigurables()
        {
            effectconf_timer_type.IsEnabled = m_IsTimedEffect && effectconf_timer_type_enable.IsChecked.GetValueOrDefault(false);
            effectconf_timer_time.IsEnabled = m_IsTimedEffect && effectconf_timer_time_enable.IsChecked.GetValueOrDefault(false);
        }

        private void OnClicked(object sender, RoutedEventArgs e)
        {
            var checkBox = (CheckBox)sender;

            if (ReferenceEquals(checkBox, effectconf_effect_shortcut_enable))
            {
                SetShortcutInputEnabled(checkBox.IsChecked.GetValueOrDefault(false));
                return;
            }

            if (!checkBox.IsChecked.GetValueOrDefault(false))
            {
                CheckEnableConfigurables();
                return;
            }

            if (ReferenceEquals(checkBox, effectconf_timer_type_enable))
                effectconf_timer_time_enable.IsChecked = false;
            else if (ReferenceEquals(checkBox, effectconf_timer_time_enable))
                effectconf_timer_type_enable.IsChecked = false;

            CheckEnableConfigurables();
        }

        public EffectData GetNewData()
        {
            m_EffectData.TimedType = effectconf_timer_type_enable.IsChecked.GetValueOrDefault(false)
                ? (effectconf_timer_type.SelectedIndex switch
                {
                    0 => EffectTimedType.Normal,
                    1 => EffectTimedType.Short,
                    2 => EffectTimedType.Permanent,
                    _ => EffectTimedType.Custom
                })
                : null;

            m_EffectData.CustomTime = effectconf_timer_time_enable.IsChecked.GetValueOrDefault(false) && int.TryParse(effectconf_timer_time.Text, out var customTime)
                ? customTime
                : null;

            m_EffectData.WeightMult = effectconf_effect_weight_mult.SelectedIndex > 0 ? effectconf_effect_weight_mult.SelectedIndex : null;

            m_EffectData.ExcludedFromVoting = effectconf_exclude_voting_state.SelectedIndex switch
            {
                1 => false,
                2 => true,
                _ => null
            };

            var customName = effectconf_effect_custom_name.Text.Trim();
            m_EffectData.CustomName = customName.Length > 0 ? customName : null;
            m_EffectData.ShortcutKeycode = effectconf_effect_shortcut_enable.IsChecked.GetValueOrDefault(false) ? m_EffectShortcut : null;

            return m_EffectData;
        }

        private void OnPrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            m_IsSaved = true;
        }
    }
}
