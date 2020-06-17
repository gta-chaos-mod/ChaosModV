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

        public EffectConfig(EffectData effectData, EffectInfo effectInfo)
        {
            InitializeComponent();

            Title = effectInfo.Name;

            m_isTimedEffect = effectInfo.IsTimed;

            if (m_isTimedEffect)
            {
                effectconf_timer_type_enable.IsChecked = effectData.EffectTimedType != (effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL);
                effectconf_timer_type.ItemsSource = new string[]
                {
                    "Normal",
                    "Short"
                };
                effectconf_timer_type.SelectedIndex = effectData.EffectTimedType == EffectTimedType.TIMED_SHORT ? 1 : 0;

                if (effectData.EffectCustomTime >= 0)
                {
                    effectconf_timer_time_enable.IsChecked = true;
                    effectconf_timer_time.Text = $"{effectData.EffectCustomTime}";
                }
            }

            effectconf_timer_permanent_enable.IsChecked = effectData.EffectPermanent;

            int[] weightItems = new int[10];
            for (int i = 0; i < 10; i++)
            {
                weightItems[i] = i + 1;
            }
            effectconf_effect_weight.ItemsSource = weightItems;
            effectconf_effect_weight.SelectedIndex = effectData.EffectWeight - 1;

            effectconf_exclude_voting_enable.IsChecked = effectData.EffectExcludedFromVoting;

            effectconf_effect_custom_name.Text = effectData.EffectCustomName;

            effectconf_mp3_label.Text += $"{effectInfo.Id}.mp3";

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
