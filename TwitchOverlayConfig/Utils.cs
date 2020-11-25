using System.Windows.Input;

namespace TwitchOverlayConfig
{
    class Utils
    {

        public static void HandleOnlyNumbersAndPeriodPreviewTextInput(TextCompositionEventArgs e)
        {
            if (e.Text.Length == 0 || !char.IsDigit(e.Text[0]) && e.Text[0]!='.')
            {
                e.Handled = true;
            }
        }

        public static void HandleOnlyNumbersPreviewTextInput(TextCompositionEventArgs e)
        {
            if (e.Text.Length == 0 || !char.IsDigit(e.Text[0]))
            {
                e.Handled = true;
            }
        }

        public static void HandleNoSpacePreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }

        public static void HandNoCopyPastePreviewExecuted(ExecutedRoutedEventArgs e)
        {
            if (e.Command == ApplicationCommands.Copy || e.Command == ApplicationCommands.Paste)
            {
                e.Handled = true;
            }
        }
    }
}
