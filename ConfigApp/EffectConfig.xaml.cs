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
        public bool IsSaved
        {
            get
            {
                return m_IsSaved;
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
            List<Key> availableKeys = new List<Key> { Key.None,
                Key.F1, Key.F2, Key.F3, Key.F4, Key.F5, Key.F6, Key.F7, Key.F8, Key.F9, Key.F10, Key.F11, Key.F12, Key.F13, Key.F14, Key.F15, Key.F16, Key.F17, Key.F18, Key.F19, Key.F20, Key.F21, Key.F22, Key.F23, Key.F24, 
                Key.NumPad0, Key.NumPad1, Key.NumPad2, Key.NumPad3, Key.NumPad4, Key.NumPad5, Key.NumPad6, Key.NumPad7, Key.NumPad8, Key.NumPad9,
                Key.Multiply, Key.Divide, Key.Subtract, Key.Add, Key.Separator
            };
            Key selectedKey = Key.None;
            if (int.TryParse(effectData.Shortcut.ToString(), out int savedWin32Key) && savedWin32Key >= 0)
            {
                selectedKey = KeyInterop.KeyFromVirtualKey(savedWin32Key);
            }

            effectconf_effect_shortcut_combo.ItemsSource = availableKeys;
            effectconf_effect_shortcut_combo.SelectedItem = selectedKey;

            CheckEnableConfigurables();
        }

        private void CustomEffectNameTextFieldTextChanged(object sender, TextChangedEventArgs e)
        {
            effectconf_effect_custom_name.Text = effectconf_effect_custom_name.Text.Replace("\"", "");
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
