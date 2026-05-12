using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Windows.System;
using Newtonsoft.Json.Linq;

namespace ConfigApp
{
    public static class Utils
    {
        public static EffectData ValueStringToEffectData(string? value)
        {
            var effectData = new EffectData();
            if (value is null)
                return effectData;

            var values = SplitCsvRespectingQuotes(value);
            if (values.Length >= 4)
            {
                effectData.Enabled = TryParseBoolFromInt(values[0]);
                effectData.TimedType = TryParseTimedType(values[1]);
                effectData.CustomTime = TryParseInt(values[2]);
                effectData.WeightMult = TryParseInt(values[3]);
            }

            if (values.Length >= 5 && TryParseInt(values[4]) is int permanent && permanent != 0)
                effectData.TimedType = Effects.EffectTimedType.Permanent;

            if (values.Length >= 6 && TryParseInt(values[5]) is int excluded)
                effectData.ExcludedFromVoting = excluded != 0;

            if (values.Length >= 7)
                effectData.CustomName = values[6] == "0" ? null : values[6].Trim('"');

            if (values.Length >= 8)
                effectData.ShortcutKeycode = TryParseInt(values[7]);

            return effectData;
        }

        private static string[] SplitCsvRespectingQuotes(string value)
        {
            var values = new List<string>();
            var current = new StringBuilder();
            var inQuotes = false;

            foreach (var character in value)
            {
                if (character == '"')
                {
                    inQuotes = !inQuotes;
                    current.Append(character);
                    continue;
                }

                if (character == ',' && !inQuotes)
                {
                    values.Add(current.ToString());
                    current.Clear();
                    continue;
                }

                current.Append(character);
            }

            values.Add(current.ToString());
            return values.ToArray();
        }

        public static EffectData ValueObjectToEffectData(JObject? value)
        {
            var effectData = new EffectData();
            if (value is null)
                return effectData;

            effectData.Enabled = value["enabled"]?.ToObject<bool?>();
            effectData.CustomTime = value["customTime"]?.ToObject<int?>();
            effectData.ExcludedFromVoting = value["excludedFromVoting"]?.ToObject<bool?>();
            effectData.TimedType = (value["permanent"]?.ToObject<bool?>() ?? false)
                ? Effects.EffectTimedType.Permanent
                : (Effects.EffectTimedType?)value["timedType"]?.ToObject<int?>();
            effectData.ShortcutKeycode = value["shortcutKeycode"]?.ToObject<int?>();
            effectData.WeightMult = value["weightMult"]?.ToObject<int?>();
            effectData.CustomName = value["customName"]?.ToObject<string?>();

            return effectData;
        }

        public static void AttachNumericTextBoxBehavior(TextBox textBox)
        {
            textBox.BeforeTextChanging += (_, eventArgs) =>
            {
                if (eventArgs.NewText.Any(character => !char.IsDigit(character)))
                    eventArgs.Cancel = true;
            };

            textBox.KeyDown += HandleNoSpaceKeyDown;
        }

        public static void HandleNoSpaceKeyDown(object sender, KeyRoutedEventArgs eventArgs)
        {
            if (eventArgs.Key == VirtualKey.Space)
                eventArgs.Handled = true;
        }

        public static CheckBox GenerateCommonCheckBox()
        {
            return new CheckBox
            {
                Width = 60f,
                Height = 20f,
                VerticalContentAlignment = VerticalAlignment.Center
            };
        }

        public static TextBox GenerateCommonNumericOnlyTextBox(int maxLength = 6, double width = 60f, double height = 20f)
        {
            var textBox = new TextBox
            {
                Width = width,
                Height = height,
                MaxLength = maxLength,
                InputScope = new InputScope
                {
                    Names = { new InputScopeName(InputScopeNameValue.Number) }
                }
            };
            AttachNumericTextBoxBehavior(textBox);
            return textBox;
        }

        public static string FormatShortcutDisplay(VirtualKey key, bool ctrl, bool shift, bool alt)
        {
            var values = new List<string>();

            if (ctrl)
                values.Add("Ctrl");
            if (shift)
                values.Add("Shift");
            if (alt)
                values.Add("Alt");

            values.Add(key switch
            {
                VirtualKey.Number0 => "0",
                VirtualKey.Number1 => "1",
                VirtualKey.Number2 => "2",
                VirtualKey.Number3 => "3",
                VirtualKey.Number4 => "4",
                VirtualKey.Number5 => "5",
                VirtualKey.Number6 => "6",
                VirtualKey.Number7 => "7",
                VirtualKey.Number8 => "8",
                VirtualKey.Number9 => "9",
                VirtualKey.Control => "Ctrl",
                VirtualKey.Shift => "Shift",
                VirtualKey.Menu => "Alt",
                _ => key.ToString()
            });

            return string.Join(" + ", values);
        }

        public static void OpenURL(string url)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true
            });
        }

        private static Effects.EffectTimedType? TryParseTimedType(string value)
        {
            return Enum.TryParse(value, out Effects.EffectTimedType timedType) && timedType != Effects.EffectTimedType.NotTimed
                ? timedType
                : null;
        }

        private static bool? TryParseBoolFromInt(string value)
        {
            return TryParseInt(value) is int intValue ? intValue != 0 : null;
        }

        private static int? TryParseInt(string value)
        {
            return int.TryParse(value, out var parsedValue) ? parsedValue : null;
        }
    }
}
