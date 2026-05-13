using System.Diagnostics;
using System.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Windows.System;
using Newtonsoft.Json.Linq;

namespace ConfigApp
{
    internal static class Utils
    {
        public static EffectData ValueStringToEffectData(string? value)
        {
            var effectData = new EffectData();
            if (value is null)
                return effectData;

            var fields = SplitCsvRespectingQuotes(value);
            if (fields.Length >= 4)
            {
                effectData.Enabled = ParseBoolFromInt(fields[0]);
                effectData.TimedType = ParseTimedType(fields[1]);
                effectData.CustomTime = ParseInt(fields[2]);
                effectData.WeightMult = ParseInt(fields[3]);
            }

            if (fields.Length >= 5 && ParseInt(fields[4]) is int permanent && permanent != 0)
                effectData.TimedType = Effects.EffectTimedType.Permanent;

            if (fields.Length >= 6 && ParseInt(fields[5]) is int excluded)
                effectData.ExcludedFromVoting = excluded != 0;

            if (fields.Length >= 7)
                effectData.CustomName = fields[6] == "0" ? null : fields[6].Trim('"');

            if (fields.Length >= 8)
                effectData.ShortcutKeycode = ParseInt(fields[7]);

            return effectData;
        }

        private static string[] SplitCsvRespectingQuotes(string value)
        {
            var fields = new List<string>();
            var current = new StringBuilder();
            var inQuotes = false;

            foreach (var character in value)
            {
                if (character == '"')
                {
                    inQuotes = !inQuotes;
                    current.Append(character);
                }
                else if (character == ',' && !inQuotes)
                {
                    fields.Add(current.ToString());
                    current.Clear();
                }
                else
                {
                    current.Append(character);
                }
            }

            fields.Add(current.ToString());
            return fields.ToArray();
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
                eventArgs.Cancel = eventArgs.NewText.Any(character => !char.IsDigit(character));
            };

            textBox.KeyDown += (_, eventArgs) =>
            {
                if (eventArgs.Key == VirtualKey.Space)
                    eventArgs.Handled = true;
            };
        }

        public static string FormatShortcutDisplay(VirtualKey key, bool ctrl, bool shift, bool alt)
        {
            var parts = new List<string>();

            if (ctrl) parts.Add("Ctrl");
            if (shift) parts.Add("Shift");
            if (alt) parts.Add("Alt");

            parts.Add(GetKeyDisplayName(key));
            return string.Join(" + ", parts);
        }

        private static string GetKeyDisplayName(VirtualKey key)
        {
            return key switch
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
            };
        }

        public static void OpenUrl(string url)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true
            });
        }

        private static Effects.EffectTimedType? ParseTimedType(string value)
        {
            return Enum.TryParse(value, out Effects.EffectTimedType timedType) && timedType != Effects.EffectTimedType.NotTimed
                ? timedType
                : null;
        }

        private static bool? ParseBoolFromInt(string value)
        {
            return ParseInt(value) is int intValue ? intValue != 0 : null;
        }

        private static int? ParseInt(string value)
        {
            return int.TryParse(value, out var parsedValue) ? parsedValue : null;
        }
    }
}
