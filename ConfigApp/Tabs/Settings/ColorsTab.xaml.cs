using Microsoft.UI;
using Microsoft.UI.Xaml.Controls;
using Windows.UI;
using System.Globalization;

namespace ConfigApp.Tabs.Settings
{
    public sealed partial class ColorsTab : UserControl, ITabLifecycle
    {
        public ColorsTab()
        {
            InitializeComponent();
            TimerBarColorPicker.Color = ColorHelper.FromArgb(0xFF, 0x40, 0x40, 0xFF);
            EffectTextColorPicker.Color = ColorHelper.FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
            EffectTimerBarColorPicker.Color = ColorHelper.FromArgb(0xFF, 0xB4, 0xB4, 0xB4);
        }

        public void OnTabSelected()
        {
        }

        public void OnLoadValues()
        {
            if (OptionsManager.ConfigFile.HasKey("EffectTimerColor"))
            {
                TimerBarColorPicker.Color = ParseColor(
                    OptionsManager.ConfigFile.ReadValue<string>("EffectTimerColor"),
                    TimerBarColorPicker.Color
                );
            }
            if (OptionsManager.ConfigFile.HasKey("EffectTextColor"))
            {
                EffectTextColorPicker.Color = ParseColor(
                    OptionsManager.ConfigFile.ReadValue<string>("EffectTextColor"),
                    EffectTextColorPicker.Color
                );
            }
            if (OptionsManager.ConfigFile.HasKey("EffectTimedTimerColor"))
            {
                EffectTimerBarColorPicker.Color = ParseColor(
                    OptionsManager.ConfigFile.ReadValue<string>("EffectTimedTimerColor"),
                    EffectTimerBarColorPicker.Color
                );
            }
        }

        public void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue(
                "EffectTimerColor",
                FormatColor(TimerBarColorPicker.Color)
            );
            OptionsManager.ConfigFile.WriteValue(
                "EffectTextColor",
                FormatColor(EffectTextColorPicker.Color)
            );
            OptionsManager.ConfigFile.WriteValue(
                "EffectTimedTimerColor",
                FormatColor(EffectTimerBarColorPicker.Color)
            );
        }

        private static Color ParseColor(string? value, Color fallback)
        {
            if (string.IsNullOrWhiteSpace(value)) return fallback;

            var normalized = value.Trim();
            if (normalized.StartsWith('#'))
            {
                normalized = normalized[1..];
            }
            if (normalized.Length == 6)
            {
                normalized = $"FF{normalized}";
            }

            return uint.TryParse(normalized, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var raw)
                ? ColorHelper.FromArgb(
                    (byte)((raw >> 24) & 0xFF),
                    (byte)((raw >> 16) & 0xFF),
                    (byte)((raw >> 08) & 0xFF),
                    (byte)(raw & 0xFF))
                : fallback;
        }

        private static string FormatColor(Color color)
        {
            return $"#{color.A:X2}{color.R:X2}{color.G:X2}{color.B:X2}";
        }
    }
}
