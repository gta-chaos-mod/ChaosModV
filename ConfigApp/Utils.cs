using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
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

        public static void HandleOnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs eventArgs)
        {
            if (eventArgs.Text.Length == 0 || !char.IsDigit(eventArgs.Text[0]))
                eventArgs.Handled = true;
        }

        public static void HandleNoSpacePreviewKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Space)
                eventArgs.Handled = true;
        }

        public static void HandleNoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs eventArgs)
        {
            if (eventArgs.Command == ApplicationCommands.Copy || eventArgs.Command == ApplicationCommands.Cut
                || eventArgs.Command == ApplicationCommands.Paste)
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
                MaxLength = maxLength
            };
            textBox.PreviewTextInput += HandleOnlyNumbersPreviewTextInput;
            textBox.AddHandler(CommandManager.PreviewExecutedEvent, new ExecutedRoutedEventHandler(HandleNoCopyPastePreviewExecuted));
            textBox.ContextMenu = null;
            textBox.AddHandler(Keyboard.PreviewKeyDownEvent, new KeyEventHandler(HandleNoSpacePreviewKeyDown));

            return textBox;
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
