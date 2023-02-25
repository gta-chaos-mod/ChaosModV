using System.Drawing;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows;
using System.Diagnostics;
using System.Windows.Input;
using System.ComponentModel;

namespace ConfigApp
{
    public class WorkshopSubmissionItem : INotifyPropertyChanged
    {
        private static BitmapSource ms_DefaultIcon;

        public event PropertyChangedEventHandler PropertyChanged;

        public string Name { get; set; }
        public string Id { get; set; }
        public string Version { get; set; }
        public string Description { get; set; }
        public int LastUpdated { get; set; }
        public string Sha256 { get; set; }
        public BitmapSource SubmissionIcon { get; set; }

        public enum SubmissionInstallState
        {
            NotInstalled,
            Installed,
            Installing,
            UpdateAvailable
        }
        private SubmissionInstallState m_InstallState = SubmissionInstallState.NotInstalled;

        public SubmissionInstallState InstallState { get
            {
                return m_InstallState;
            }
            set
            {
                m_InstallState = value;

                switch (value)
                {
                    case SubmissionInstallState.NotInstalled:
                        InstallButtonText = "Install";
                        InstallButtonEnabled = true;
                        break;
                    case SubmissionInstallState.Installed:
                        InstallButtonText = "Installed";
                        InstallButtonEnabled = false;
                        break;
                    case SubmissionInstallState.Installing:
                        InstallButtonText = "Installing";
                        InstallButtonEnabled = false;
                        break;
                    case SubmissionInstallState.UpdateAvailable:
                        InstallButtonText = "Update";
                        InstallButtonEnabled = true;
                        break;
                }

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonText)));
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonEnabled)));
            }
        }

        public ICommand InstallButtonCommand {
            get
            {
                return new WorkshopInstallHandler(this);
            }
        }
        public string InstallButtonText { get; private set; } = "Install";
        public bool InstallButtonEnabled { get; private set; } = true;

        public WorkshopSubmissionItem()
        {
            if (ms_DefaultIcon == null)
            {
                using (Icon ico = Icon.ExtractAssociatedIcon(Process.GetCurrentProcess().MainModule.FileName))
                {
                    ms_DefaultIcon = Imaging.CreateBitmapSourceFromHIcon(ico.Handle, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
                }
            }

            SubmissionIcon = ms_DefaultIcon;
        }
    }
}
