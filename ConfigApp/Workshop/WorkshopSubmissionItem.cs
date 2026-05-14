using System.ComponentModel;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Input;
using Microsoft.UI.Xaml;

namespace ConfigApp.Workshop
{
    internal sealed class SearchTerm
    {
        public string Term { get; }
        public string? FileName { get; }

        public SearchTerm(string term, string? filename = null)
        {
            Term = term;
            FileName = filename;
        }
    }

    internal sealed class WorkshopSubmissionItem : INotifyPropertyChanged
    {
        private static readonly Regex s_SearchFieldRegex = new(@"(?:Name|ScriptId|EffectId)\s*=\s*""((?:\\""|[^""])*)""", RegexOptions.Compiled);

        private readonly WorkshopSubmissionFileHandler m_FileHandler;
        private readonly ICommand m_InstallButtonCommand;
        private readonly ICommand m_InfoButtonCommand;
        private readonly ICommand m_SettingsButtonCommand;

        public event PropertyChangedEventHandler? PropertyChanged;

        public string? Id { get; private init; }
        public string? Name { get; init; }
        public string? Author { get; init; }
        public string? Description { get; init; }
        public string? Version { get; init; }
        public int? LastUpdated { get; init; }
        public string? Sha256 { get; init; }
        public bool IsAlien { get; set; }
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

        public ICommand InstallButtonCommand => m_InstallButtonCommand;
        public string InstallButtonText { get; private set; } = "Install";
        public bool InstallButtonEnabled { get; private set; } = true;
        public ICommand InfoButtonCommand => m_InfoButtonCommand;
        public ICommand SettingsButtonCommand => m_SettingsButtonCommand;
        public Visibility SettingsButtonVisibility { get; private set; } = Visibility.Collapsed;

        public WorkshopSubmissionItem(string id)
        {
            Id = id;
            m_FileHandler = new(this);
            m_InstallButtonCommand = new WorkshopInstallHandler(this);
            m_InfoButtonCommand = new WorkshopInfoHandler(this);
            m_SettingsButtonCommand = new WorkshopSettingsHandler(this, m_FileHandler);
        }

        public void Refresh()
        {
            m_FileHandler.ReloadFiles();
            UpdateSearchTerms();
        }

        public void UpdateSearchTerms()
        {
            SearchTerms.Clear();

            AddSearchTermIfNotEmpty(Name);
            AddSearchTermIfNotEmpty(Description);
            AddSearchTermIfNotEmpty(Author);

            foreach (var file in m_FileHandler.GetSubmissionFiles())
            {
                SearchTerms.Add(new(file.Name, file.Name));
                if (file.EffectData?.CustomName is not null)
                    SearchTerms.Add(new(file.EffectData.CustomName, file.Name));

                if (file.Type != WorkshopSubmissionFileType.Script)
                    continue;

                try
                {
                    var fullPath = Path.Combine(m_FileHandler.SubmissionDirectory, file.Name);
                    foreach (var line in File.ReadAllLines(fullPath))
                    {
                        var match = s_SearchFieldRegex.Match(line);
                        if (match.Success)
                            SearchTerms.Add(new(match.Groups[1].Value, file.Name));
                    }
                }
                catch (IOException)
                {
                }
            }
        }

        private void AddSearchTermIfNotEmpty(string? term)
        {
            if (!string.IsNullOrWhiteSpace(term))
                SearchTerms.Add(new SearchTerm(term));
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
            OnPropertyChanged(nameof(InstallButtonText));
            OnPropertyChanged(nameof(InstallButtonEnabled));
            OnPropertyChanged(nameof(SettingsButtonVisibility));
        }

        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
