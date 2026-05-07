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

        public static async Task ShowMessageAsync(string content, string title = "ChaosModV", string closeButtonText = "OK")
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
                CloseButtonText = closeButtonText,
                DefaultButton = ContentDialogButton.Close
            };

            ApplyXamlRoot(dialog);
            await dialog.ShowAsync();
        }

        public static async Task<bool> ShowYesNoAsync(string content, string title = "ChaosModV", string yesButtonText = "Yes", string noButtonText = "No")
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
                PrimaryButtonText = yesButtonText,
                CloseButtonText = noButtonText,
                DefaultButton = ContentDialogButton.Primary
            };

            ApplyXamlRoot(dialog);
            return await dialog.ShowAsync() == ContentDialogResult.Primary;
        }

        public static async Task<bool> ShowOkCancelAsync(string content, string title = "ChaosModV", string okButtonText = "OK", string cancelButtonText = "Cancel")
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
                PrimaryButtonText = okButtonText,
                CloseButtonText = cancelButtonText,
                DefaultButton = ContentDialogButton.Primary
            };

            ApplyXamlRoot(dialog);
            return await dialog.ShowAsync() == ContentDialogResult.Primary;
        }

        private static void ApplyXamlRoot(ContentDialog dialog)
        {
            if (s_XamlRoot is not null)
                dialog.XamlRoot = s_XamlRoot;
        }
    }
}