using System.ComponentModel;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Input;
using Microsoft.UI.Xaml;

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
            IsInFile = filename is not null;
            FileName = filename ?? string.Empty;
        }

        public static implicit operator SearchTerm(string term) => new(term);
    }

    public class WorkshopSubmissionItem : INotifyPropertyChanged
    {
        private readonly WorkshopSubmissionFileHandler m_FileHandler;

        public event PropertyChangedEventHandler? PropertyChanged = null;

        public string? Id { get; private init; } = null;
        public string? Name { get; init; } = null;
        public string? Author { get; init; } = null;
        public string? Description { get; init; } = null;
        public string? Version { get; init; } = null;
        public int? LastUpdated { get; init; } = null;
        public string? Sha256 { get; init; } = null;
        public bool IsAlien { get; set; } = false;
        public List<SearchTerm> SearchTerms { get; } = new();
        public List<string> HighlightedFiles { get; } = new();

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
                UpdateInstallUi(value);
                NotifyInstallPropertiesChanged();
            }
        }

        public ICommand InstallButtonCommand => new WorkshopInstallHandler(this);
        public string InstallButtonText { get; private set; } = "Install";
        public bool InstallButtonEnabled { get; private set; } = true;
        public ICommand InfoButtonCommand => new WorkshopInfoHandler(this);
        public ICommand SettingsButtonCommand => new WorkshopSettingsHandler(this, m_FileHandler);
        public Visibility SettingsButtonVisibility { get; private set; } = Visibility.Collapsed;

        public WorkshopSubmissionItem(string id)
        {
            Id = id;
            m_FileHandler = new(this);
        }

        public void Refresh()
        {
            m_FileHandler.ReloadFiles();
            UpdateSearchTerms();
        }

        public void UpdateSearchTerms()
        {
            SearchTerms.Clear();

            AddSearchTermIfPresent(Name);
            AddSearchTermIfPresent(Description);
            AddSearchTermIfPresent(Author);

            foreach (var file in m_FileHandler.GetSubmissionFiles())
            {
                SearchTerms.Add(new(file.Name, file.Name));
                if (file.EffectData?.CustomName is not null)
                    SearchTerms.Add(new(file.EffectData.CustomName, file.Name));

                if (file.Type != WorkshopSubmissionFileType.Script)
                    continue;

                try
                {
                    foreach (var line in File.ReadAllLines(m_FileHandler.SubmissionDirectory + file.Name))
                    {
                        var match = Regex.Match(line, @"(?:Name|ScriptId|EffectId)\s*=\s*""((?:\\""|[^""])+)""");
                        if (match.Success)
                            SearchTerms.Add(new(match.Groups[1].Value, file.Name));
                    }
                }
                catch (FileNotFoundException)
                {
                }
                catch (IOException)
                {
                }
            }
        }

        private void AddSearchTermIfPresent(string? value)
        {
            if (value is not null)
                SearchTerms.Add(value);
        }

        private void UpdateInstallUi(SubmissionInstallState value)
        {
            switch (value)
            {
            case SubmissionInstallState.NotInstalled:
                InstallButtonText = "Install";
                InstallButtonEnabled = !IsAlien;
                SettingsButtonVisibility = Visibility.Collapsed;
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
        }

        private void NotifyInstallPropertiesChanged()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonText)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(InstallButtonEnabled)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SettingsButtonVisibility)));
        }
    }
}
