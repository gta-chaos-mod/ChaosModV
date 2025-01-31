using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;

namespace ConfigApp
{
    public class WorkshopSubmissionItem : INotifyPropertyChanged
    {
        private static BitmapSource? ms_DefaultIcon = null;

        public event PropertyChangedEventHandler? PropertyChanged = null;

        public string? Id { get; set; } = null;
        public string? Name { get; set; } = null;
        public string? Author { get; set; } = null;
        public string? Description { get; set; } = null;
        public string? Version { get; set; } = null;
        public int? LastUpdated { get; set; } = null;
        public string? Sha256 { get; set; } = null;
        public BitmapSource? SubmissionIcon { get; set; } = null;
        public bool IsAlien { get; set; } = false;

        // In order for sorting
        public enum SubmissionInstallState
        {
            UpdateAvailable,
            Installed,
            NotInstalled,
            Installing,
            Removing
        }
        private SubmissionInstallState m_InstallState = SubmissionInstallState.NotInstalled;

        public SubmissionInstallState InstallState
        {
            get => m_InstallState;
            set
            {
                m_InstallState = value;

                switch (value)
                {
                case SubmissionInstallState.NotInstalled:
                    InstallButtonText = "Install";
                    InstallButtonEnabled = !IsAlien;
                    SettingsButtonVisibility = Visibility.Hidden;
                    break;
                case SubmissionInstallState.Installed:
                    InstallButtonText = "Remove";
                    InstallButtonEnabled = true;
                    SettingsButtonVisibility = Visibility.Visible;
                    break;
                case SubmissionInstallState.Installing:
                    InstallButtonText = "Installing";
                    InstallButtonEnabled = false;
                    break;
                case SubmissionInstallState.UpdateAvailable:
                    InstallButtonText = "Update";
                    InstallButtonEnabled = !IsAlien;
                    SettingsButtonVisibility = Visibility.Visible;
                    break;
                case SubmissionInstallState.Removing:
                    InstallButtonText = "Removing";
                    InstallButtonEnabled = false;
                    break;
                }

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonText)));
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonEnabled)));

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SettingsButtonVisibility)));
            }
        }

        public ICommand InstallButtonCommand
        {
            get => new WorkshopInstallHandler(this);
        }
        public string InstallButtonText { get; private set; } = "Install";
        public bool InstallButtonEnabled { get; private set; } = true;

        public ICommand InfoButtonCommand
        {
            get => new WorkshopInfoHandler(this);
        }

        public ICommand SettingsButtonCommand
        {
            get => new WorkshopSettingsHandler(this);
        }
        public Visibility SettingsButtonVisibility { get; private set; } = Visibility.Hidden;

        public WorkshopSubmissionItem()
        {
            if (ms_DefaultIcon == null)
            {
                var fileName = Process.GetCurrentProcess().MainModule?.FileName;
                if (fileName is not null)
                {
                    try
                    {
                        using var ico = Icon.ExtractAssociatedIcon(fileName);
                        if (ico is not null)
                            ms_DefaultIcon = Imaging.CreateBitmapSourceFromHIcon(ico.Handle, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
                    }
                    catch (System.PlatformNotSupportedException)
                    {

                    }
                }
            }

            SubmissionIcon = ms_DefaultIcon;
        }
    }
}
