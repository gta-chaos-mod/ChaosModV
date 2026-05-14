using System.Security.Cryptography;
using System.Text;
using ConfigApp.Infrastructure;
using ConfigApp.Workshop;
using Microsoft.UI.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ZstdSharp;

namespace ConfigApp.Tabs
{
    public sealed partial class WorkshopTab : UserControl, ITabLifecycle
    {
        private const string SUBMISSIONS_CACHED_FILENAME = "workshop/submissions_cached.json.zst";

        private enum SortingMode
        {
            Name,
            LastUpdated,
            Author
        }

        private SortingMode m_SortingMode = SortingMode.Name;
        private readonly List<WorkshopSubmissionItem> m_WorkshopSubmissionItems = [];
        private bool m_HasLoaded = false;

        public WorkshopTab()
        {
            InitializeComponent();
        }

        private void ApplySortAndFilter()
        {
            if (SearchBox is null || SortInstalledFirstToggle is null || ItemsPanel is null)
                return;

            var normalizedSearchText = SearchBox.Text?.Trim().ToLowerInvariant() ?? string.Empty;

            UpdateHighlightedFiles(normalizedSearchText);

            IEnumerable<WorkshopSubmissionItem> items = string.IsNullOrWhiteSpace(normalizedSearchText)
                ? m_WorkshopSubmissionItems
                : m_WorkshopSubmissionItems.Where(item => IsSearchMatch(item, normalizedSearchText));

            items = m_SortingMode switch
            {
                SortingMode.Name => items.OrderBy(item => item.Name?.ToLowerInvariant()),
                SortingMode.LastUpdated => items.OrderByDescending(item => item.LastUpdated),
                SortingMode.Author => items.OrderBy(item => item.Author?.ToLowerInvariant()),
                _ => throw new NotImplementedException()
            };

            if (SortInstalledFirstToggle.IsChecked.GetValueOrDefault(true))
                items = items.OrderBy(item => item.InstallState);

            RenderSubmissionItems(items);
        }

        private static bool IsSearchMatch(WorkshopSubmissionItem item, string normalizedSearchText)
        {
            return item.SearchTerms.Any(term => term.Term.Contains(normalizedSearchText, StringComparison.InvariantCultureIgnoreCase));
        }

        private void UpdateHighlightedFiles(string normalizedSearchText)
        {
            foreach (var item in m_WorkshopSubmissionItems)
            {
                item.HighlightedFiles.Clear();

                if (string.IsNullOrWhiteSpace(normalizedSearchText))
                    continue;

                foreach (var fileName in item.SearchTerms
                    .Where(term => term.IsInFile && term.Term.Contains(normalizedSearchText, StringComparison.InvariantCultureIgnoreCase))
                    .Select(term => term.FileName)
                    .Distinct(StringComparer.OrdinalIgnoreCase))
                {
                    item.HighlightedFiles.Add(fileName);
                }
            }
        }

        private void RenderSubmissionItems(IEnumerable<WorkshopSubmissionItem> items)
        {
            ItemsPanel.Children.Clear();

            var itemsList = items.ToList();
            if (itemsList.Count == 0)
            {
                ItemsPanel.Children.Add(new TextBlock
                {
                    Text = "No workshop submissions match the current filter.",
                    Margin = new Thickness(0, 12, 0, 0)
                });
                return;
            }

            foreach (var item in itemsList)
                ItemsPanel.Children.Add(CreateSubmissionCard(item));
        }

        private static Border CreateSubmissionCard(WorkshopSubmissionItem item)
        {
            var stack = new StackPanel
            {
                Spacing = 8
            };

            stack.Children.Add(new TextBlock
            {
                Text = item.Name ?? "Unnamed submission",
                FontSize = 18,
                FontWeight = FontWeights.SemiBold,
                TextWrapping = TextWrapping.Wrap
            });

            stack.Children.Add(new TextBlock
            {
                Text = $"By {item.Author ?? "Unknown"} • {item.Version ?? "Unknown version"}",
                Opacity = 0.8
            });

            if (item.LastUpdated.HasValue)
            {
                stack.Children.Add(new TextBlock
                {
                    Text = $"Last Updated: {DateTimeOffset.FromUnixTimeSeconds(item.LastUpdated.Value):u}",
                    Opacity = 0.8
                });
            }

            stack.Children.Add(new TextBlock
            {
                Text = item.Description ?? "No description",
                TextWrapping = TextWrapping.Wrap
            });

            if (item.HighlightedFiles.Count > 0)
            {
                stack.Children.Add(new TextBlock
                {
                    Text = $"Matches in files: {string.Join(", ", item.HighlightedFiles)}",
                    TextWrapping = TextWrapping.Wrap,
                    Foreground = new SolidColorBrush(Microsoft.UI.Colors.DarkGoldenrod)
                });
            }

            stack.Children.Add(CreateButtonRow(item));

            return new Border
            {
                BorderThickness = new Thickness(1),
                BorderBrush = new SolidColorBrush(Microsoft.UI.Colors.LightGray),
                CornerRadius = new CornerRadius(8),
                Padding = new Thickness(12),
                Margin = new Thickness(0, 0, 0, 12),
                Child = stack
            };
        }

        private static StackPanel CreateButtonRow(WorkshopSubmissionItem item)
        {
            var buttonRow = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 8,
                HorizontalAlignment = HorizontalAlignment.Right
            };

            buttonRow.Children.Add(CreateActionButton("Info", () => item.InfoButtonCommand.Execute(null)));

            if (item.SettingsButtonVisibility == Visibility.Visible)
                buttonRow.Children.Add(CreateActionButton("Settings", () => item.SettingsButtonCommand.Execute(null)));

            var installButton = new Button
            {
                Content = item.InstallButtonText,
                IsEnabled = item.InstallButtonEnabled
            };
            installButton.Click += (_, _) => item.InstallButtonCommand.Execute(null);
            buttonRow.Children.Add(installButton);

            return buttonRow;
        }

        private static Button CreateActionButton(string content, Action onClick)
        {
            var button = new Button
            {
                Content = content
            };
            button.Click += (_, _) => onClick();
            return button;
        }

        private async Task ParseWorkshopSubmissionsFileAsync(byte[] compressedFileContent)
        {
            ParseRemoteSubmissionEntries(compressedFileContent);
            await ParseLocalSubmissionEntriesAsync();
            ApplySortAndFilter();
        }

        private void ParseRemoteSubmissionEntries(byte[] compressedFileContent)
        {
            var decompressor = new Decompressor();
            var decompressed = decompressor.Unwrap(compressedFileContent);
            var fileContent = Encoding.UTF8.GetString(decompressed.ToArray());
            var json = JObject.Parse(fileContent);

            m_WorkshopSubmissionItems.Clear();

            if (json["submissions"] is not JObject submissions)
                return;

            foreach (var submission in submissions)
                SubmitWorkshopSubmissionData(submission.Key, submission.Value as JObject, false);
        }

        private async Task ParseLocalSubmissionEntriesAsync()
        {
            Directory.CreateDirectory("workshop");

            foreach (var directory in Directory.GetDirectories("workshop/"))
            {
                var id = Path.GetFileName(directory);
                var metadataPath = $"{directory}/metadata.json";

                if (!File.Exists(metadataPath))
                {
                    await AppDialog.ShowMessageAsync($"Local submission \"{id}\" is missing a metadata.json.");
                    continue;
                }

                try
                {
                    var json = JObject.Parse(File.ReadAllText(metadataPath));
                    SubmitWorkshopSubmissionData(id, json, true);
                }
                catch (JsonException)
                {
                    await AppDialog.ShowMessageAsync($"Local submission \"{id}\" has a corrupt metadata.json.");
                }
            }
        }

        private void SubmitWorkshopSubmissionData(string id, JObject? submissionData, bool isLocal)
        {
            if (submissionData is null || string.IsNullOrWhiteSpace(id))
                return;

            var version = GetDataItem(submissionData, "version", string.Empty);
            if (string.IsNullOrEmpty(version))
                return;

            var lastUpdated = GetDataItem(submissionData, "lastupdated", 0);
            var sha256 = GetDataItem(submissionData, "sha256", string.Empty);

            var duplicateSubmissionItem = m_WorkshopSubmissionItems.FirstOrDefault(submissionItem => submissionItem.Id == id);
            if (duplicateSubmissionItem is not null)
            {
                if (isLocal)
                {
                    if (duplicateSubmissionItem.Version != version || duplicateSubmissionItem.LastUpdated != lastUpdated || duplicateSubmissionItem.Sha256 != sha256)
                        duplicateSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.UpdateAvailable;
                    else
                        duplicateSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                }
                return;
            }

            var submissionItem = new WorkshopSubmissionItem(id)
            {
                Name = GetDataItem(submissionData, "name", "No Name"),
                Author = GetDataItem(submissionData, "author", "No Author"),
                Description = GetDataItem(submissionData, "description", "No Description"),
                Version = $"v{version}",
                LastUpdated = lastUpdated,
                Sha256 = sha256
            };

            submissionItem.UpdateSearchTerms();

            if (isLocal)
            {
                submissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                submissionItem.IsAlien = true;
            }

            submissionItem.PropertyChanged += (_, _) => ApplySortAndFilter();
            m_WorkshopSubmissionItems.Add(submissionItem);
        }

        private static T GetDataItem<T>(JObject submissionData, string key, T defaultValue)
        {
            var token = submissionData[key];
            if (token is null || token.Type == JTokenType.Null)
                return defaultValue;

            try
            {
                var value = token.ToObject<T>();
                return value is null ? defaultValue : value;
            }
            catch (JsonException)
            {
                return defaultValue;
            }
        }

        private async Task ForceRefreshWorkshopContentFromRemote()
        {
            var domain = OptionsManager.WorkshopFile.ReadValue("WorkshopCustomUrl", Info.WORKSHOP_DEFAULT_URL);

            using HttpClient httpClient = new();
            try
            {
                if (await TryLoadUnchangedCachedSubmissionsAsync(domain, httpClient))
                    return;

                await DownloadAndCacheSubmissionsAsync(domain, httpClient);
            }
            catch (HttpRequestException)
            {
                await AppDialog.ShowMessageAsync("Error occured while trying to fetch submissions from server! Please try again!");
            }
            catch (InvalidOperationException)
            {
                await AppDialog.ShowMessageAsync($"Specified workshop URL ({domain}) is invalid!");
            }
            catch (Exception exception) when (exception is JsonException || exception is ZstdException)
            {
                await AppDialog.ShowMessageAsync("Remote server provided a malformed master submissions file! Can not fetch available submissions.");
            }
        }

        private async Task<bool> TryLoadUnchangedCachedSubmissionsAsync(string domain, HttpClient httpClient)
        {
            if (!File.Exists(SUBMISSIONS_CACHED_FILENAME))
                return false;

            var hashResult = await httpClient.GetAsync($"{domain}/workshop/fetch_submissionshash");
            if (!hashResult.IsSuccessStatusCode)
                return false;

            var remoteHash = await hashResult.Content.ReadAsStringAsync();
            var localContent = File.ReadAllBytes(SUBMISSIONS_CACHED_FILENAME);
            var localHash = Convert.ToHexString(SHA256.HashData(localContent));

            if (!string.Equals(remoteHash, localHash, StringComparison.OrdinalIgnoreCase))
                return false;

            await ParseWorkshopSubmissionsFileAsync(localContent);
            return true;
        }

        private async Task DownloadAndCacheSubmissionsAsync(string domain, HttpClient httpClient)
        {
            var submissionsResult = await httpClient.GetAsync($"{domain}/workshop/fetch_submissions");
            if (!submissionsResult.IsSuccessStatusCode)
            {
                await AppDialog.ShowMessageAsync("Remote server provided no master submissions file! Can not fetch available submissions.");
                return;
            }

            var submissionsCompressedResult = await submissionsResult.Content.ReadAsByteArrayAsync();
            await ParseWorkshopSubmissionsFileAsync(submissionsCompressedResult);
            File.WriteAllBytes(SUBMISSIONS_CACHED_FILENAME, submissionsCompressedResult);
        }

        private async void OnSettingsClick(object sender, RoutedEventArgs eventArgs)
        {
            var dialog = new WorkshopSettingsDialog();
            await dialog.ShowAsync();
            if (dialog.IsSaved)
                await ForceRefreshWorkshopContentFromRemote();
        }

        private async void OnRefreshClick(object sender, RoutedEventArgs eventArgs)
        {
            RefreshButton.IsEnabled = false;

            try
            {
                foreach (var item in m_WorkshopSubmissionItems)
                    item.Refresh();

                await ForceRefreshWorkshopContentFromRemote();
            }
            finally
            {
                RefreshButton.IsEnabled = true;
            }
        }

        private void OnTextChangeSearch(object sender, TextChangedEventArgs eventArgs)
        {
            ApplySortAndFilter();
        }

        private void OnSortingModeBoxSelectionChanged(object sender, SelectionChangedEventArgs eventArgs)
        {
            m_SortingMode = (SortingMode)SortingModeBox.SelectedIndex;
            ApplySortAndFilter();
        }

        private void OnSortInstalledFirstClick(object sender, RoutedEventArgs e)
        {
            ApplySortAndFilter();
        }

        public async void OnTabSelected()
        {
            if (m_HasLoaded)
                return;

            m_HasLoaded = true;
            await LoadInitialWorkshopContentAsync();
        }

        public void OnLoadValues()
        {
        }

        public void OnSaveValues()
        {
        }

        private async Task LoadInitialWorkshopContentAsync()
        {
            var cachedFileContent = TryReadCachedSubmissions();

            if (cachedFileContent is null)
            {
                await ForceRefreshWorkshopContentFromRemote();
                return;
            }

            try
            {
                await ParseWorkshopSubmissionsFileAsync(cachedFileContent);
            }
            catch (JsonException)
            {
                await ForceRefreshWorkshopContentFromRemote();
            }
        }

        private static byte[]? TryReadCachedSubmissions()
        {
            if (!File.Exists(SUBMISSIONS_CACHED_FILENAME))
                return null;

            try
            {
                return File.ReadAllBytes(SUBMISSIONS_CACHED_FILENAME);
            }
            catch (IOException)
            {
                return null;
            }
        }
    }
}
