using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Infrastructure
{
    public static class AppDialog
    {
        private static XamlRoot? s_XamlRoot;

        public static void Initialize(XamlRoot? xamlRoot)
        {
            s_XamlRoot = xamlRoot;
        }

        public static void ApplyToDialog(ContentDialog dialog)
        {
            dialog.XamlRoot = ResolveXamlRoot();
        }

        public static Task ShowMessageAsync(string content, string title = "ChaosModV", string closeButtonText = "OK")
        {
            return ShowDialogAsync(content, title, closeButtonText: closeButtonText);
        }

        public static Task<bool> ShowYesNoAsync(string content, string title = "ChaosModV", string yesButtonText = "Yes", string noButtonText = "No")
        {
            return ShowDialogAsync(content, title, yesButtonText, noButtonText);
        }

        public static Task<bool> ShowOkCancelAsync(string content, string title = "ChaosModV", string okButtonText = "OK", string cancelButtonText = "Cancel")
        {
            return ShowDialogAsync(content, title, okButtonText, cancelButtonText);
        }

        private static async Task<bool> ShowDialogAsync(string content, string title, string primaryButtonText = "", string closeButtonText = "OK")
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
                CloseButtonText = closeButtonText
            };

            if (!string.IsNullOrEmpty(primaryButtonText))
                dialog.PrimaryButtonText = primaryButtonText;

            ApplyToDialog(dialog);
            return await dialog.ShowAsync() == ContentDialogResult.Primary;
        }

        private static XamlRoot? ResolveXamlRoot()
        {
            if (s_XamlRoot is not null)
                return s_XamlRoot;

            if (App.MainWindow?.Content is FrameworkElement rootElement)
                return rootElement.XamlRoot;

            return null;
        }
    }
}
