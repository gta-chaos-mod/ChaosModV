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

        public EffectConfig(EffectData effectData, EffectInfo effectInfo, int lang)
        {
            InitializeComponent();

            effect_rename.Content = Lang.effect_config("effect_rename", lang);
            effect_override_timer_type.Content = Lang.effect_config("effect_override_timer_type", lang);
            effect_override_timer_manual.Content = Lang.effect_config("effect_override_timer_manual", lang);
            effect_make_permanent.Content = Lang.effect_config("effect_make_permanent", lang);
            effectconf_effect_weight_mult_title.Content = Lang.effect_config("effectconf_effect_weight_mult_title", lang);
            effectconf_exclude_voting_enable_title.Content = Lang.effect_config("effectconf_exclude_voting_enable_title", lang);
            effectconf_mp3_label.Text = Lang.effect_config("effectconf_mp3_label", lang) + $"{effectInfo.Id}.mp3";
            button_save.Content = Lang.effect_config("button_save", lang);

            Title = effectInfo.Name;

            m_isTimedEffect = effectInfo.IsTimed;

            if (m_isTimedEffect)
            {
                effectconf_timer_type_enable.IsChecked = effectData.TimedType != (effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL);
                effectconf_timer_type.ItemsSource = new string[]
                {
                    "Normal",
                    "Short"
                };
                effectconf_timer_type.SelectedIndex = effectData.TimedType == EffectTimedType.TIMED_SHORT ? 1 : 0;

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

            effectconf_effect_custom_name.Text = $"{(Title == effectData.CustomName ? "" : effectData.CustomName)}";

            effectconf_mp3_label.Text += $"{effectInfo.Id}.mp3";

            // Meta Effect Handling

            if (effectInfo.EffectCategory == EffectCategory.META)
            {
                effectconf_exclude_voting_enable_title.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.Visibility = Visibility.Hidden;
                effectconf_exclude_voting_enable.IsChecked = false;
            }

            CheckEnableConfigurables();
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
