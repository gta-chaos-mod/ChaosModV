using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Newtonsoft.Json.Linq;

namespace ConfigApp
{
    public static class Utils
    {
        public static EffectData ValuesArrayToEffectData<T>(T? value)
        {
            var effectData = new EffectData();

            if (value is null)
                return effectData;

            dynamic[] values;
            bool isNewConfig = true;
            switch (value)
            {
            case JArray arrayValue:
                values = arrayValue.Children().ToArray();
                break;
            case string strValue:
                // Split by comma, ignoring commas in between quotation marks
                values = Regex.Split(strValue, ",(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))");
                isNewConfig = false;
                break;
            default:
                throw new NotImplementedException("Invalid Value Type");
            }

            // .ini configs stored everything (except CustomName) as an int
            // Also has compatibility checks as very ancient mod versions had less options

            if (values.Length >= 4)
            {
                int enabled = isNewConfig ? (bool)values[0] ? 1 : 0 : int.Parse(values[0]);
                effectData.Enabled = enabled != 0;

                Effects.EffectTimedType timedType = (Effects.EffectTimedType)(isNewConfig ? values[1] : Enum.Parse(typeof(Effects.EffectTimedType), values[1]));
                effectData.TimedType = timedType != Effects.EffectTimedType.NotTimed ? timedType : null;

                int customTime = isNewConfig ? values[2] : int.Parse(values[2]);
                effectData.CustomTime = customTime;

                int weightMult = isNewConfig ? values[3] : int.Parse(values[3]);
                effectData.WeightMult = weightMult;
            }

            if (values.Length >= 5)
            {
                int isPermanent = isNewConfig ? (bool)values[4] ? 1 : 0 : int.Parse(values[4]);
                if (isPermanent != 0)
                    effectData.TimedType = Effects.EffectTimedType.Permanent;
            }

            if (values.Length >= 6)
            {
                int excludedFromVoting = isNewConfig ? (bool)values[5] ? 1 : 0 : int.Parse(values[5]);
                effectData.ExcludedFromVoting = excludedFromVoting != 0;
            }

            if (values.Length >= 7)
                effectData.CustomName = values[6] == "0" ? null : ((string)values[6]).Trim('\"');

            if (values.Length >= 8)
            {
                int shortcut = isNewConfig ? values[7] : int.Parse(values[7]);
                effectData.ShortcutKeycode = shortcut;
            }

            // New JSON-exclusive stored values below here, meaning we can properly use the stored type

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
    }
}
