using System.Diagnostics;
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

            // Split by comma, ignoring commas in between quotation marks
            var values = Regex.Split(value, ",(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))");

            /* Has compatibility checks as previous mod versions had less options */

            if (values.Length >= 4)
            {
                if (int.TryParse(values[0], out int enabled))
                    effectData.Enabled = enabled != 0;

                if (Enum.TryParse(values[1], out Effects.EffectTimedType timedType))
                    effectData.TimedType = timedType != Effects.EffectTimedType.NotTimed ? timedType : null;
                if (int.TryParse(values[2], out int customTime))
                    effectData.CustomTime = customTime;
                if (int.TryParse(values[3], out int weightMult))
                    effectData.WeightMult = weightMult;
            }

            if (values.Length >= 5 && int.TryParse(values[4], out int tmp) && tmp != 0)
                effectData.TimedType = Effects.EffectTimedType.Permanent;

            if (values.Length >= 6 && int.TryParse(values[5], out tmp))
                effectData.ExcludedFromVoting = tmp != 0;

            if (values.Length >= 7)
                effectData.CustomName = values[6] == "0" ? null : values[6].Trim('\"');

            if (values.Length >= 8 && int.TryParse(values[7], out int shortcut))
                effectData.ShortcutKeycode = shortcut;

            return effectData;
        }

        public static EffectData ValueObjectToEffectData(JObject? value)
        {
            var effectData = new EffectData();

            if (value is null)
                return effectData;

            effectData.Enabled = value["enabled"]?.ToObject<bool?>();
            effectData.CustomTime = value["customTime"]?.ToObject<int?>();
            effectData.ExcludedFromVoting = value["excludedFromVoting"]?.ToObject<bool?>();
            bool permanent = value["permanent"]?.ToObject<bool?>() ?? false;
            effectData.ShortcutKeycode = value["shortcutKeycode"]?.ToObject<int?>();
            effectData.TimedType = permanent ? Effects.EffectTimedType.Permanent : (Effects.EffectTimedType?)value["timedType"]?.ToObject<int?>();
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
            return new CheckBox()
            {
                Width = 60f,
                Height = 20f,
                VerticalContentAlignment = VerticalAlignment.Center
            };
        }

        public static TextBox GenerateCommonNumericOnlyTextBox(int maxLength = 6, double width = 60f, double height = 20f)
        {
            var textBox = new TextBox()
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

        public static bool IsNumeric<T>(this T value)
        {
            if (value is null)
                return false;

            var t = Nullable.GetUnderlyingType(value.GetType()) ?? value.GetType();
            return t.IsPrimitive || t == typeof(decimal);
        }

        public static void OpenURL(string url)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true
            });
        }
    }
}
