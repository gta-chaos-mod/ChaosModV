using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp
{
    internal partial class WorkshopSettingsDialog : ContentDialog
    {
        public bool IsSaved { get; private set; }

        public WorkshopSettingsDialog()
        {
            InitializeComponent();
            AppDialog.ApplyToDialog(this);

            workshop_custom_url.Text = OptionsManager.WorkshopFile.ReadValue<string>("WorkshopCustomUrl");
            workshop_custom_url.PlaceholderText = Info.WORKSHOP_DEFAULT_URL;
        }

        private void OnPrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            IsSaved = true;

            OptionsManager.WorkshopFile.WriteValue("WorkshopCustomUrl", workshop_custom_url.Text);
            OptionsManager.WorkshopFile.WriteFile();
        }
    }
}
