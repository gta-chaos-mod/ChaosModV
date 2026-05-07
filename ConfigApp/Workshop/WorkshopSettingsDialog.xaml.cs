using Microsoft.UI.Xaml.Controls;

namespace ConfigApp
{
    public partial class WorkshopSettingsDialog : ContentDialog
    {
        private bool m_IsSaved = false;

        public bool IsSaved
        {
            get => m_IsSaved;
        }

        public WorkshopSettingsDialog()
        {
            InitializeComponent();

            workshop_custom_url.Text = OptionsManager.WorkshopFile.ReadValue<string>("WorkshopCustomUrl");
            workshop_custom_url.PlaceholderText = Info.WORKSHOP_DEFAULT_URL;
        }

        private void OnPrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            m_IsSaved = true;

            OptionsManager.WorkshopFile.WriteValue("WorkshopCustomUrl", workshop_custom_url.Text);
            OptionsManager.WorkshopFile.WriteFile();
        }
    }
}
