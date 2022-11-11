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
        private bool m_isTimedEffect;
        private bool m_IsSaved = false;
        private int m_EffectShortcut; // Win32Key + 2^10 (if CTRL) + 2^9 (if Shift) + 2^8 (if Alt)
        public bool IsSaved
        {
            get
            {
                return m_IsSaved;
            }
        }
        public int EffectShortcut
        {
            get
            {
                return m_EffectShortcut;
            }
        }

        public EffectConfig(string effectId, EffectData effectData, EffectInfo effectInfo)
        {
            InitializeComponent();

            Title = effectInfo.Name;

            m_isTimedEffect = effectInfo.IsTimed;

            if (m_isTimedEffect)
            {
                effectconf_timer_type_enable.IsChecked = effectData.TimedType != (effectInfo.IsShort ? EffectTimedType.TimedShort : EffectTimedType.TimedNormal);
                effectconf_timer_type.ItemsSource = new string[]
                {
                    "Normal",
                    "Short"
                };
                effectconf_timer_type.SelectedIndex = effectData.TimedType == EffectTimedType.TimedShort ? 1 : 0;

                if (effectData.CustomTime >= 0)
                {
                    effectconf_timer_time_enable.IsChecked = true;
                    effectconf_timer_time.Text = $"{effectData.CustomTime}";
                }
            }

            effectconf_timer_permanent_enable.IsChecked = effectData.Permanent;

            int[] weightItems = new int[10];
            for (int i = 0; i < 10; i++)
            {
                weightItems[i] = i + 1;
            }
            effectconf_effect_weight_mult.ItemsSource = weightItems;
            effectconf_effect_weight_mult.SelectedIndex = effectData.WeightMult - 1;

            effectconf_exclude_voting_enable.IsChecked = effectData.ExcludedFromVoting;

            effectconf_effect_custom_name.Text = effectData.CustomName;
            effectconf_effect_custom_name.TextChanged += CustomEffectNameTextFieldTextChanged;

            effectconf_mp3_label.Text = $@"
                Sound to play when this effect gets activated: chaosmod/sounds/{effectId}.mp3
                Or create the following folder and drop mp3 files in there to play a random one: chaosmod/sounds/{effectId}
                ";

            // Meta Effect Handling

            if (effectInfo.EffectCategory == EffectCategory.Meta)
            {
                effectconf_exclude_voting_enable_title.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.IsChecked = false;
            }

            // Shortcut
            if (int.TryParse(effectData.Shortcut.ToString(), out int savedWin32Key))
            {
                if (savedWin32Key > 0)
                {
                    Key key = KeyInterop.KeyFromVirtualKey(savedWin32Key % 256);
                    var modifiers = ModifierKeys.None;
                    if ((savedWin32Key & (1 << 10)) != 0) modifiers |= ModifierKeys.Control;
                    if ((savedWin32Key & (1 << 9)) != 0) modifiers |= ModifierKeys.Shift;
                    if ((savedWin32Key & (1 << 8)) != 0) modifiers |= ModifierKeys.Alt;

                    SetEffectShortcut(key, modifiers);
                } else if (savedWin32Key == 0)
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

        private void EffectShortcutTextFieldPreviewKeyDown(object sender, KeyEventArgs e)   // PreviewKeyDown so that text editing shortcuts such as "CTRL+C" are possible
        {
            e.Handled = true;

            var key = e.Key;
            var modifiers = Keyboard.Modifiers;

            if (key == Key.Escape || key == Key.Back)
            {
                effectconf_effect_shortcut_input.Text = "None";
                m_EffectShortcut = 0;
                return;
            }

            if (key == Key.System)
            {
                key = e.SystemKey;
            }

            if (key == Key.LeftCtrl || key == Key.RightCtrl || key == Key.LeftShift 
                || key == Key.RightShift || key == Key.LeftAlt || key == Key.RightAlt 
                || key == Key.LWin || key == Key.RWin || key == Key.Apps)                                   // Don't want a shortcut with any of these as the main key
            {
                return;
            }

            SetEffectShortcut(key, modifiers);
        }

        private void SetEffectShortcut(Key key, ModifierKeys modifiers)
        {
            m_EffectShortcut = 0;
            var text = new System.Text.StringBuilder();

            if (modifiers.HasFlag(ModifierKeys.Control))            // CTRL is marked as true even if only the AltGr Key is pressed
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
            if (!m_isTimedEffect)
            {
                effectconf_timer_type_enable.IsEnabled = false;
                effectconf_timer_time_enable.IsEnabled = false;
                effectconf_timer_permanent_enable.IsEnabled = false;
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
                    effectconf_timer_permanent_enable.IsChecked = false;
                }
                else if (sender == effectconf_timer_time_enable)
                {
                    effectconf_timer_type_enable.IsChecked = false;
                    effectconf_timer_permanent_enable.IsChecked = false;
                }
                else if (sender == effectconf_timer_permanent_enable)
                {
                    effectconf_timer_type_enable.IsChecked = false;
                    effectconf_timer_time_enable.IsChecked = false;
                }
            }

            CheckEnableConfigurables();
        }

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersPreviewTextInput(e);
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
        {
            Utils.HandleNoSpacePreviewKeyDown(e);
        }

        private void NoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Utils.HandleNoCopyPastePreviewExecuted(e);
        }

        private void OnSave(object sender, RoutedEventArgs e)
        {
            m_IsSaved = true;

            Close();
        }
    }
}
