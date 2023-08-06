using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ConfigApp
{
    public static class Utils
    {
        public static void HandleOnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs eventArgs)
        {
            if (eventArgs.Text.Length == 0 || !char.IsDigit(eventArgs.Text[0]))
            {
                eventArgs.Handled = true;
            }
        }

        public static void HandleNoSpacePreviewKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Space)
            {
                eventArgs.Handled = true;
            }
        }

        public static void HandleNoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs eventArgs)
        {
            if (eventArgs.Command == ApplicationCommands.Copy || eventArgs.Command == ApplicationCommands.Cut
                || eventArgs.Command == ApplicationCommands.Paste)
            {
                eventArgs.Handled = true;
            }
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
    }
}
