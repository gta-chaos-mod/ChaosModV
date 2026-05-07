using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Windows.UI;

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
                Width = 240f,
                Height = 320f,
                Color = defaultColor,
                IsAlphaEnabled = false,
                IsColorSpectrumVisible = true,
                IsColorPreviewVisible = true
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

            grid.PushRowSpacedPair("Timer bar color", m_TimerBarColor = GenerateCommonColorPicker(ColorHelper.FromArgb(0xFF, 0x40, 0x40, 0xFF)));
            grid.PopRow();

            grid.PushRowSpacedPair("Effect text color", m_EffectTextColor = GenerateCommonColorPicker(ColorHelper.FromArgb(0xFF, 0xFF, 0xFF, 0xFF)));
            grid.PushRowSpacedPair("Effect timer bar color", m_EffectTimerBarColor = GenerateCommonColorPicker(ColorHelper.FromArgb(0xFF, 0xB4, 0xB4, 0xB4)));
            grid.PopRow();

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (OptionsManager.ConfigFile.HasKey("EffectTimerColor") && m_TimerBarColor is not null)
                m_TimerBarColor.Color = ParseColor(OptionsManager.ConfigFile.ReadValue<string>("EffectTimerColor"), m_TimerBarColor.Color);
            if (OptionsManager.ConfigFile.HasKey("EffectTextColor") && m_EffectTextColor is not null)
                m_EffectTextColor.Color = ParseColor(OptionsManager.ConfigFile.ReadValue<string>("EffectTextColor"), m_EffectTextColor.Color);
            if (OptionsManager.ConfigFile.HasKey("EffectTimedTimerColor") && m_EffectTimerBarColor is not null)
                m_EffectTimerBarColor.Color = ParseColor(OptionsManager.ConfigFile.ReadValue<string>("EffectTimedTimerColor"), m_EffectTimerBarColor.Color);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EffectTimerColor", FormatColor(m_TimerBarColor?.Color));
            OptionsManager.ConfigFile.WriteValue("EffectTextColor", FormatColor(m_EffectTextColor?.Color));
            OptionsManager.ConfigFile.WriteValue("EffectTimedTimerColor", FormatColor(m_EffectTimerBarColor?.Color));
        }

        private static Color ParseColor(string? value, Color fallback)
        {
            if (string.IsNullOrWhiteSpace(value))
                return fallback;

            var normalized = value.Trim();
            if (normalized.StartsWith("#"))
                normalized = normalized[1..];
            if (normalized.Length == 6)
                normalized = $"FF{normalized}";

            return uint.TryParse(normalized, System.Globalization.NumberStyles.HexNumber, null, out var raw)
                ? ColorHelper.FromArgb((byte)((raw >> 24) & 0xFF), (byte)((raw >> 16) & 0xFF), (byte)((raw >> 8) & 0xFF), (byte)(raw & 0xFF))
                : fallback;
        }

        private static string? FormatColor(Color? color)
        {
            if (color is null)
                return null;

            return $"#{color.Value.A:X2}{color.Value.R:X2}{color.Value.G:X2}{color.Value.B:X2}";
        }
    }
}
