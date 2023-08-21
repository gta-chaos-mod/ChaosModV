using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

using static ConfigApp.Effects;

namespace ConfigApp
{
    public partial class EffectConfig : Window
    {
        private EffectData m_EffectData;
        private bool m_IsTimedEffect = false;
        private bool m_IsSaved = false;
        private int? m_EffectShortcut; // Win32Key + 2^10 (if CTRL) + 2^9 (if Shift) + 2^8 (if Alt)

        public bool IsSaved
        {
            get
            {
                return m_IsSaved;
            }
        }

        public EffectConfig(string effectId, EffectData? effectData, EffectInfo effectInfo)
        {
            InitializeComponent();

            if (effectData == null)
            {
                effectData = new EffectData();
            }

            Title = effectInfo.Name;

            m_EffectData = effectData;
            m_IsTimedEffect = effectInfo.IsTimed;

            if (m_IsTimedEffect)
            {
                effectconf_timer_type_enable.IsChecked = m_EffectData.TimedType != null;
                effectconf_timer_type.ItemsSource = new string[]
                {
                    "Normal",
                    "Short",
                    "Permanent"
                };
                switch (m_EffectData.TimedType.GetValueOrDefault(EffectTimedType.Normal))
                {
                    case EffectTimedType.Normal:
                        effectconf_timer_type.SelectedIndex = 0;
                        break;
                    case EffectTimedType.Short:
                        effectconf_timer_type.SelectedIndex = 1;
                        break;
                    case EffectTimedType.Permanent:
                        effectconf_timer_type.SelectedIndex = 2;
                        break;
                    default:
                        effectconf_timer_type.SelectedIndex = 0;
                        break;
                }

                if (m_EffectData.CustomTime >= 0)
                {
                    effectconf_timer_time_enable.IsChecked = true;
                    effectconf_timer_time.Text = $"{m_EffectData.CustomTime}";
                }
            }

            effectconf_effect_weight_mult.ItemsSource = new List<string>()
            {
                "Default", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
            };
            effectconf_effect_weight_mult.SelectedIndex = m_EffectData.WeightMult.GetValueOrDefault(0);

            effectconf_exclude_voting_enable.IsChecked = m_EffectData.ExcludedFromVoting.GetValueOrDefault(false);

            effectconf_effect_custom_name.Text = m_EffectData.CustomName;
            effectconf_effect_custom_name.TextChanged += CustomEffectNameTextFieldTextChanged;

            if (!string.IsNullOrWhiteSpace(effectId))
            {
                effectconf_mp3_label.Text = $@"
                    Sound to play when this effect gets activated: chaosmod/sounds/{effectId}.mp3
                    Or create the following folder and drop mp3 files in there to play a random one: chaosmod/sounds/{effectId}
                ";
            }

            // Meta Effect Handling

            if (effectInfo.EffectCategory == EffectCategory.Meta)
            {
                effectconf_exclude_voting_enable_title.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.IsChecked = false;
            }

            // Shortcut
            if (int.TryParse(m_EffectData.ShortcutKeycode.ToString(), out int savedWin32Key))
            {
                if (savedWin32Key > 0)
                {
                    Key key = KeyInterop.KeyFromVirtualKey(savedWin32Key % 256);
                    var modifiers = ModifierKeys.None;
                    if ((savedWin32Key & (1 << 10)) != 0)
                    {
                        modifiers |= ModifierKeys.Control;
                    }
                    if ((savedWin32Key & (1 << 9)) != 0)
                    {
                        modifiers |= ModifierKeys.Shift;
                    }
                    if ((savedWin32Key & (1 << 8)) != 0)
                    {
                        modifiers |= ModifierKeys.Alt;
                    }

                    SetEffectShortcut(key, modifiers);
                }
                else if (savedWin32Key == 0)
                {
                    effectconf_effect_shortcut_input.Text = "None";
                }
            }

            CheckEnableConfigurables();
        }

        private void CustomEffectNameTextFieldTextChanged(object sender, TextChangedEventArgs e)
        {
            effectconf_effect_custom_name.Text = effectconf_effect_custom_name.Text.Replace("\"", "");
        }

        // PreviewKeyDown so that text editing shortcuts such as "CTRL+C" are possible
        private void EffectShortcutTextFieldPreviewKeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;

            var key = e.Key;
            var modifiers = Keyboard.Modifiers;

            if (key == Key.Escape || key == Key.Back)
            {
                effectconf_effect_shortcut_input.Text = "None";
                m_EffectShortcut = null;
                return;
            }

            if (key == Key.System)
            {
                key = e.SystemKey;
            }

            // Don't want a shortcut with any of these as the main key
            if (key == Key.LeftCtrl || key == Key.RightCtrl || key == Key.LeftShift
                || key == Key.RightShift || key == Key.LeftAlt || key == Key.RightAlt
                || key == Key.LWin || key == Key.RWin || key == Key.Apps)
            {
                return;
            }

            SetEffectShortcut(key, modifiers);
        }

        private void SetEffectShortcut(Key key, ModifierKeys modifiers)
        {
            m_EffectShortcut = 0;
            var text = new System.Text.StringBuilder();

            // CTRL is marked as true even if only the AltGr Key is pressed
            if (modifiers.HasFlag(ModifierKeys.Control))
            {
                text.Append("Ctrl + ");
                m_EffectShortcut += 1 << 10;
            }
            if (modifiers.HasFlag(ModifierKeys.Shift))
            {
                text.Append("Shift + ");
                m_EffectShortcut += 1 << 9;
            }
            if (modifiers.HasFlag(ModifierKeys.Alt))
            {
                text.Append("Alt + ");
                m_EffectShortcut += 1 << 8;
            }

            text.Append(new KeyConverter().ConvertToString(key));

            effectconf_effect_shortcut_input.Text = text.ToString();
            m_EffectShortcut += KeyInterop.VirtualKeyFromKey(key);
        }

        private void CheckEnableConfigurables()
        {
            if (!m_IsTimedEffect)
            {
                effectconf_timer_type_enable.IsEnabled = false;
                effectconf_timer_time_enable.IsEnabled = false;
            }

            effectconf_timer_type.IsEnabled = effectconf_timer_type_enable.IsChecked.Value;
            effectconf_timer_time.IsEnabled = effectconf_timer_time_enable.IsChecked.Value;
        }

        private void OnClicked(object sender, RoutedEventArgs e)
        {
            if (((CheckBox)sender).IsChecked.Value)
            {
                if (sender == effectconf_timer_type_enable)
                {
                    effectconf_timer_time_enable.IsChecked = false;
                }
                else if (sender == effectconf_timer_time_enable)
                {
                    effectconf_timer_type_enable.IsChecked = false;
                }
            }

            CheckEnableConfigurables();
        }

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersPreviewTextInput(sender, e);
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
        {
            Utils.HandleNoSpacePreviewKeyDown(sender, e);
        }

        private void NoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Utils.HandleNoCopyPastePreviewExecuted(sender, e);
        }

        public EffectData GetNewData()
        {
            if (effectconf_timer_type_enable.IsChecked.HasValue && effectconf_timer_type_enable.IsChecked.Value)
            {
                switch (effectconf_timer_type.SelectedIndex)
                {
                    case 0:
                        m_EffectData.TimedType = EffectTimedType.Normal;
                        break;
                    case 1:
                        m_EffectData.TimedType = EffectTimedType.Short;
                        break;
                    case 2:
                        m_EffectData.TimedType = EffectTimedType.Permanent;
                        break;
                    default:
                        m_EffectData.TimedType = null;
                        break;
                }
            }
            else
            {
                m_EffectData.TimedType = null;
            }

            m_EffectData.CustomTime = effectconf_timer_time_enable.IsChecked.HasValue && effectconf_timer_time_enable.IsChecked.Value
                ? effectconf_timer_time.Text.Length > 0 ? int.Parse(effectconf_timer_time.Text) : null : null;
            m_EffectData.WeightMult = effectconf_effect_weight_mult.SelectedIndex > 0 ? effectconf_effect_weight_mult.SelectedIndex : null;
            m_EffectData.ExcludedFromVoting = effectconf_exclude_voting_enable.IsChecked.HasValue && effectconf_exclude_voting_enable.IsChecked.Value
                ? true : null; // Currently the assumption is that every effect is voteable in the mod anyways so don't waste space where unneeded storing this is false
            m_EffectData.CustomName = effectconf_effect_custom_name.Text.Trim().Length > 0 ? effectconf_effect_custom_name.Text.Trim() : null;
            m_EffectData.ShortcutKeycode = m_EffectShortcut;

            return m_EffectData;
        }

        private void OnSave(object sender, RoutedEventArgs e)
        {
            m_IsSaved = true;

            Close();
        }
    }
}
