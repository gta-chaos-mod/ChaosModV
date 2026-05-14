using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Infrastructure
{
    internal static class AppDialog
    {
        internal static void ApplyToDialog(ContentDialog dialog)
        {
            dialog.XamlRoot = GetXamlRoot();
        }

        internal static Task ShowMessageAsync(string content, string title = "ChaosModV", string closeButtonText = "Ok")
        {
            return ShowDialogAsync(content, title, closeButtonText: closeButtonText);
        }

        internal static Task<bool> ShowYesNoAsync(string content, string title = "ChaosModV", string yesButtonText = "Yes", string noButtonText = "No")
        {
            return ShowDialogAsync(content, title, yesButtonText, noButtonText);
        }

        internal static Task<bool> ShowOkCancelAsync(string content, string title = "ChaosModV", string okButtonText = "Ok", string cancelButtonText = "Cancel")
        {
            return ShowDialogAsync(content, title, okButtonText, cancelButtonText);
        }

        private static async Task<bool> ShowDialogAsync(string content, string title, string primaryButtonText = "", string closeButtonText = "Ok")
        {
            var dialog = new ContentDialog
            {
                Title = title,
                Content = new TextBlock
                {
                    Text = content,
                    TextWrapping = TextWrapping.Wrap,
                    MaxWidth = 560
                },
                DefaultButton = string.IsNullOrEmpty(primaryButtonText) ? ContentDialogButton.Close : ContentDialogButton.Primary,
                CloseButtonText = closeButtonText,
                XamlRoot = GetXamlRoot()
            };

            if (!string.IsNullOrEmpty(primaryButtonText))
                dialog.PrimaryButtonText = primaryButtonText;

            return await dialog.ShowAsync() == ContentDialogResult.Primary;
        }

        private static XamlRoot? GetXamlRoot()
        {
            return (App.MainWindow?.Content as FrameworkElement)?.XamlRoot;
        }
    }
}
