using System.Windows;

namespace ConfigApp
{
    public partial class WorkshopSettingsDialog : Window
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
            workshop_custom_url.Watermark = Info.WORKSHOP_DEFAULT_URL;
        }

        private void OnSave(object sender, RoutedEventArgs e)
        {
            m_IsSaved = true;

            OptionsManager.WorkshopFile.WriteValue("WorkshopCustomUrl", workshop_custom_url.Text);
            OptionsManager.WorkshopFile.WriteFile();

            Close();
        }
    }
}
