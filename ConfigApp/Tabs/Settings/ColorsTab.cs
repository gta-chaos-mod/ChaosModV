using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Xceed.Wpf.Toolkit;

namespace ConfigApp.Tabs.Settings
{
    public class ColorsTab : Tab
    {
        private ColorPicker? m_TimerBarColor = null;
        private ColorPicker? m_EffectTextColor = null;
        private ColorPicker? m_EffectTimerBarColor = null;

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

            grid.PushRowSpacedPair("Timer bar color", m_TimerBarColor = GenerateCommonColorPicker(Color.FromRgb(0x40, 0x40, 0xFF)));
            grid.PopRow();

            grid.PushRowSpacedPair("Effect text color", m_EffectTextColor = GenerateCommonColorPicker(Color.FromRgb(0xFF, 0xFF, 0xFF)));
            grid.PushRowSpacedPair("Effect timer bar color", m_EffectTimerBarColor = GenerateCommonColorPicker(Color.FromRgb(0xB4, 0xB4, 0xB4)));
            grid.PopRow();

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (OptionsManager.ConfigFile.HasKey("EffectTimerColor") && m_TimerBarColor is not null)
                m_TimerBarColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTimerColor"));
            if (OptionsManager.ConfigFile.HasKey("EffectTextColor") && m_EffectTextColor is not null)
                m_EffectTextColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTextColor"));
            if (OptionsManager.ConfigFile.HasKey("EffectTimedTimerColor") && m_EffectTimerBarColor is not null)
                m_EffectTimerBarColor.SelectedColor = (Color)ColorConverter.ConvertFromString(OptionsManager.ConfigFile.ReadValue("EffectTimedTimerColor"));
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EffectTimerColor", m_TimerBarColor?.SelectedColor.ToString());
            OptionsManager.ConfigFile.WriteValue("EffectTextColor", m_EffectTextColor?.SelectedColor.ToString());
            OptionsManager.ConfigFile.WriteValue("EffectTimedTimerColor", m_EffectTimerBarColor?.SelectedColor.ToString());
        }
    }
}
