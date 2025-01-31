using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;

namespace ConfigApp.Workshop
{
    public class SearchTerm
    {
        public string Term { get; }
        public bool IsInFile { get; }
        public string FileName { get; }

        public SearchTerm(string term, string? filename = null)
        {
            Term = term;
            if (filename is not null)
            {
                IsInFile = true;
                FileName = filename;
            }
            else
            {
                IsInFile = false;
                FileName = "";
            }
        }

        public static implicit operator SearchTerm(string term) => new(term);
    }

    public class WorkshopSubmissionItem : INotifyPropertyChanged
    {
        private static BitmapSource? ms_DefaultIcon = null;

        private readonly WorkshopSubmissionFileHandler m_FileHandler;

        public event PropertyChangedEventHandler? PropertyChanged = null;

        public string? Id { get; private init; } = null;
        public string? Name { get; init; } = null;
        public string? Author { get; init; } = null;
        public string? Description { get; init; } = null;
        public string? Version { get; init; } = null;
        public int? LastUpdated { get; init; } = null;
        public string? Sha256 { get; init; } = null;
        public BitmapSource? SubmissionIcon { get; init; } = null;
        public bool IsAlien { get; set; } = false;
        public List<SearchTerm> SearchTerms { get; } = new();
        public List<string> HighlightedFiles { get; } = new List<string>();

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
            get => new WorkshopSettingsHandler(this, m_FileHandler);
        }
        public Visibility SettingsButtonVisibility { get; private set; } = Visibility.Hidden;

        public void Refresh()
        {
            m_FileHandler.ReloadFiles();
            UpdateSearchTerms();
        }

        public void UpdateSearchTerms()
        {
            SearchTerms.Clear();

            if (Name is not null)
                SearchTerms.Add(Name);
            if (Description is not null)
                SearchTerms.Add(Description);
            if (Author is not null)
                SearchTerms.Add(Author);

            foreach (var file in m_FileHandler.GetSubmissionFiles())
            {
                SearchTerms.Add(new(file.Name, file.Name));
                if (file.EffectData?.CustomName is not null)
                {
                    SearchTerms.Add(new(file.EffectData.CustomName, file.Name));
                }

                if (file.Type == WorkshopSubmissionFileType.Script)
                {
                    try
                    {
                        foreach (var line in File.ReadAllLines(m_FileHandler.SubmissionDirectory + file.Name))
                        {
                            var match = Regex.Match(line, @"(?:Name|ScriptId)\s*=\s*""((?:\\""|[^""])+)""");
                            if (match.Success)
                            {
                                SearchTerms.Add(new(match.Groups[1].Value, file.Name));
                            }
                        }
                    }
                    catch (Exception exception) when (exception is IOException || exception is FileNotFoundException)
                    {
                        continue;
                    }
                }
            }
        }

        public WorkshopSubmissionItem(string id)
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

            Id = id;

            m_FileHandler = new(this);
        }
    }
}
