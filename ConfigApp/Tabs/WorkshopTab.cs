using System.IO;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using ConfigApp.Workshop;
using ConfigApp.Infrastructure;
using Microsoft.CSharp.RuntimeBinder;
using Microsoft.UI.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ZstdSharp;

namespace ConfigApp.Tabs
{
    public class WorkshopTab : Tab
    {
        public const string SUBMISSIONS_CACHED_FILENAME = "workshop/submissions_cached.json.zst";

        enum SortingMode
        {
            Name,
            LastUpdated,
            Author
        }
        private readonly Dictionary<SortingMode, string> m_SortingModeLabels = new()
        {
            { SortingMode.Name, "Name" },
            { SortingMode.LastUpdated, "Last Updated" },
            { SortingMode.Author, "Author" }
        };
        private SortingMode m_SortingMode = SortingMode.Name;

        private readonly List<WorkshopSubmissionItem> m_WorkshopSubmissionItems = new();
        private bool m_HasLoaded = false;

        private CheckBox? m_SortIntalledFirstToggle = null;
        private TextBox? m_SearchBox = null;
        private StackPanel? m_ItemsPanel = null;

        private void ApplySortAndFilter()
        {
            var filteredText = m_SearchBox?.Text.Trim().ToLowerInvariant();

            foreach (var item in m_WorkshopSubmissionItems)
            {
                item.HighlightedFiles.Clear();

                if (!string.IsNullOrWhiteSpace(filteredText))
                {
                    foreach (var term in item.SearchTerms)
                    {
                        if (term.Term.ToLowerInvariant().Contains(filteredText))
                        {
                            if (term.IsInFile && !item.HighlightedFiles.Contains(term.FileName))
                                item.HighlightedFiles.Add(term.FileName);
                        }
                    }
                }
            }

            IEnumerable<WorkshopSubmissionItem> items = string.IsNullOrWhiteSpace(filteredText)
                ? m_WorkshopSubmissionItems
                : m_WorkshopSubmissionItems.Where(item => item.SearchTerms.Any(term => term.Term.ToLowerInvariant().Contains(filteredText)));

            items = m_SortingMode switch
            {
                SortingMode.Name => items.OrderBy(item => item.Name?.ToLowerInvariant()),
                SortingMode.LastUpdated => items.OrderByDescending(item => item.LastUpdated),
                SortingMode.Author => items.OrderBy(item => item.Author?.ToLowerInvariant()),
                _ => throw new NotImplementedException(),
            };

            if (m_SortIntalledFirstToggle == null || m_SortIntalledFirstToggle.IsChecked.GetValueOrDefault(true))
                items = items.OrderBy(item => item.InstallState);

            RenderSubmissionItems(items.ToList());
        }

        private void RenderSubmissionItems(List<WorkshopSubmissionItem> items)
        {
            if (m_ItemsPanel is null)
                return;

            m_ItemsPanel.Children.Clear();

            if (items.Count == 0)
            {
                m_ItemsPanel.Children.Add(new TextBlock
                {
                    Text = "No workshop submissions match the current filter.",
                    Margin = new Thickness(0, 12, 0, 0)
                });
                return;
            }

            foreach (var item in items)
            {
                var headerText = new TextBlock
                {
                    Text = item.Name ?? "Unnamed submission",
                    FontSize = 18,
                    FontWeight = FontWeights.SemiBold,
                    TextWrapping = TextWrapping.Wrap
                };

                var stack = new StackPanel
                {
                    Spacing = 8
                };
                stack.Children.Add(headerText);
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

                var buttonRow = new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    Spacing = 8,
                    HorizontalAlignment = HorizontalAlignment.Right
                };

                var infoButton = new Button { Content = "Info" };
                infoButton.Click += (_, _) => item.InfoButtonCommand.Execute(null);
                buttonRow.Children.Add(infoButton);

                if (item.SettingsButtonVisibility == Visibility.Visible)
                {
                    var settingsButton = new Button { Content = "Settings" };
                    settingsButton.Click += (_, _) => item.SettingsButtonCommand.Execute(null);
                    buttonRow.Children.Add(settingsButton);
                }

                var installButton = new Button
                {
                    Content = item.InstallButtonText,
                    IsEnabled = item.InstallButtonEnabled
                };
                installButton.Click += (_, _) => item.InstallButtonCommand.Execute(null);
                buttonRow.Children.Add(installButton);
                stack.Children.Add(buttonRow);

                m_ItemsPanel.Children.Add(new Border
                {
                    BorderThickness = new Thickness(1),
                    BorderBrush = new SolidColorBrush(Microsoft.UI.Colors.LightGray),
                    CornerRadius = new CornerRadius(8),
                    Padding = new Thickness(12),
                    Margin = new Thickness(0, 0, 0, 12),
                    Child = stack
                });
            }
        }

        private async Task ParseWorkshopSubmissionsFileAsync(byte[] compressedFileContent)
        {
            void submitWorkshopSubmissionData(dynamic submissionData, bool isLocal)
            {
                T getDataItem<T>(dynamic item, T defaultValue)
                {
                    try
                    {
                        return item;
                    }
                    catch (RuntimeBinderException)
                    {
                        return defaultValue;
                    }
                }

                var id = getDataItem<string>(submissionData.id, string.Empty);
                if (string.IsNullOrEmpty(id))
                    return;

                var version = getDataItem<string>(submissionData.version, string.Empty);
                if (string.IsNullOrEmpty(version))
                    return;

                var lastUpdated = getDataItem<int>(submissionData.lastupdated, 0);
                var sha256 = getDataItem<string>(submissionData.sha256, string.Empty);

                var duplicateSubmissionItem = m_WorkshopSubmissionItems.FirstOrDefault((submissionItem) => { return submissionItem.Id == id; });
                if (duplicateSubmissionItem != null)
                {
                    if (isLocal)
                        if (duplicateSubmissionItem.Version != version || duplicateSubmissionItem.LastUpdated != lastUpdated || duplicateSubmissionItem.Sha256 != sha256)
                            duplicateSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.UpdateAvailable;
                        else
                            duplicateSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                    return;
                }

                var submissionItem = new WorkshopSubmissionItem(id)
                {
                    Name = getDataItem<string>(submissionData.name, "No Name"),
                    Author = getDataItem<string>(submissionData.author, "No Author"),
                    Description = getDataItem<string>(submissionData.description, "No Description"),
                    Version = $"v{version}",
                    LastUpdated = lastUpdated,
                    Sha256 = sha256,
                };

                submissionItem.UpdateSearchTerms();

                // Remote submissions are fetched before local ones so this submission only exists locally
                if (isLocal)
                {
                    submissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                    submissionItem.IsAlien = true;
                }

                submissionItem.PropertyChanged += (_, _) => ApplySortAndFilter();

                m_WorkshopSubmissionItems.Add(submissionItem);
            }

            {
                var decompressor = new Decompressor();
                var decompressed = decompressor.Unwrap(compressedFileContent);
                var fileContent = Encoding.UTF8.GetString(decompressed.ToArray());

                var json = JObject.Parse(fileContent);

                // Only clear after trying to parse
                m_WorkshopSubmissionItems.Clear();

                var dict = json["submissions"]?.ToObject<Dictionary<string, dynamic>?>();
                if (dict is not null)
                {
                    foreach (var submissionObject in dict)
                    {
                        var submissionId = submissionObject.Key;

                        var submissionData = submissionObject.Value;
                        submissionData.id = submissionId;

                        submitWorkshopSubmissionData(submissionData, false);
                    }
                }
            }

            Directory.CreateDirectory("workshop");

            foreach (var directory in Directory.GetDirectories("workshop/"))
            {
                var id = Path.GetFileName(directory);

                if (!File.Exists($"{directory}/metadata.json"))
                {
                    await AppDialog.ShowMessageAsync($"Local submission \"{id}\" is missing a metadata.json.");
                    continue;
                }

                try
                {
                    var json = JObject.Parse(File.ReadAllText($"{directory}/metadata.json"));

                    var submissionData = json.ToObject<dynamic>();
                    if (submissionData == null)
                        continue;
                    submissionData.id = id;

                    submitWorkshopSubmissionData(submissionData, true);
                }
                catch (Exception exception) when (exception is JsonException || exception is ZstdException)
                {
                    await AppDialog.ShowMessageAsync($"Local submission \"{id}\" has a corrupt metadata.json.");
                    continue;
                }
            }

            ApplySortAndFilter();
        }

        private async Task ForceRefreshWorkshopContentFromRemote()
        {
            var domain = OptionsManager.WorkshopFile.ReadValue("WorkshopCustomUrl", Info.WORKSHOP_DEFAULT_URL);

            HttpClient httpClient = new();
            try
            {
                if (File.Exists(SUBMISSIONS_CACHED_FILENAME))
                {
                    var hashResult = await httpClient.GetAsync($"{domain}/workshop/fetch_submissionshash");
                    if (hashResult.IsSuccessStatusCode)
                    {
                        var remoteHash = await hashResult.Content.ReadAsStringAsync();
                        var localContent = File.ReadAllBytes(SUBMISSIONS_CACHED_FILENAME);
                        using var sha256 = SHA256.Create();
                        if (remoteHash.ToLower() == Convert.ToHexString(sha256.ComputeHash(localContent)).ToLower())
                        {
                            await ParseWorkshopSubmissionsFileAsync(localContent);
                            return;
                        }
                    }
                }

                var submissionsResult = await httpClient.GetAsync($"{domain}/workshop/fetch_submissions");
                if (!submissionsResult.IsSuccessStatusCode)
                    await AppDialog.ShowMessageAsync("Remote server provided no master submissions file! Can not fetch available submissions.");
                else
                {
                    var submissionsCompressedResult = await submissionsResult.Content.ReadAsByteArrayAsync();

                    await ParseWorkshopSubmissionsFileAsync(submissionsCompressedResult);

                    // Cache submissions
                    File.WriteAllBytes(SUBMISSIONS_CACHED_FILENAME, submissionsCompressedResult);
                }
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

        private async void OnSettingsClick(object sender, RoutedEventArgs eventArgs)
        {
            var dialog = new WorkshopSettingsDialog();
            await dialog.ShowAsync();
            if (dialog.IsSaved)
                await ForceRefreshWorkshopContentFromRemote();
        }

        private async void OnRefreshClick(object sender, RoutedEventArgs eventArgs)
        {
            var button = (Button)sender;

            button.IsEnabled = false;
            foreach (var item in m_WorkshopSubmissionItems)
                item.Refresh();

            await ForceRefreshWorkshopContentFromRemote();
            button.IsEnabled = true;
        }

        private void OnTextChangeSearch(object sender, TextChangedEventArgs eventArgs)
        {
            ApplySortAndFilter();
        }

        private void OnSortingModeBoxSelectionChanged(object sender, SelectionChangedEventArgs eventArgs)
        {
            var box = (ComboBox)sender;

            m_SortingMode = (SortingMode)box.SelectedIndex;
            ApplySortAndFilter();
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));

            SetRowHeight(new GridLength());

            var headerGrid = new Grid();
            headerGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            headerGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            var controlsRow = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 8,
                HorizontalAlignment = HorizontalAlignment.Left
            };
            controlsRow.Children.Add(new TextBlock
            {
                VerticalAlignment = VerticalAlignment.Center,
                Text = "Sort By:"
            });
            var sortingModeBox = new ComboBox()
            {
                Width = 100f,
                SelectedIndex = 0
            };
            foreach (var value in m_SortingModeLabels.Values)
                sortingModeBox.Items.Add(value);
            sortingModeBox.SelectionChanged += OnSortingModeBoxSelectionChanged;
            controlsRow.Children.Add(sortingModeBox);

            m_SortIntalledFirstToggle = new CheckBox()
            {
                VerticalAlignment = VerticalAlignment.Center,
                IsChecked = true,
                Content = "Show installed first"
            };
            m_SortIntalledFirstToggle.Click += (sender, eventArgs) => { SortSubmissionItems(); };
            controlsRow.Children.Add(m_SortIntalledFirstToggle);
            headerGrid.Children.Add(controlsRow);

            var rightHeader = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 8,
                HorizontalAlignment = HorizontalAlignment.Right
            };
            rightHeader.SetValue(Grid.ColumnProperty, 1);

            m_SearchBox = new TextBox()
            {
                Width = 250f,
                PlaceholderText = "Search"
            };
            m_SearchBox.TextChanged += OnTextChangeSearch;
            rightHeader.Children.Add(m_SearchBox);

            var settingsButton = new Button()
            {
                Content = "Settings"
            };
            ToolTipService.SetToolTip(settingsButton, "Settings");
            settingsButton.Click += OnSettingsClick;
            rightHeader.Children.Add(settingsButton);

            var refreshButton = new Button()
            {
                Content = "Refresh"
            };
            ToolTipService.SetToolTip(refreshButton, "Refresh");
            refreshButton.Click += OnRefreshClick;
            rightHeader.Children.Add(refreshButton);
            headerGrid.Children.Add(rightHeader);

            PushRowElement(headerGrid);
            PopRow();

            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var scrollViewer = new ScrollViewer()
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch
            };

            m_ItemsPanel = new StackPanel()
            {
                Spacing = 0
            };
            scrollViewer.Content = m_ItemsPanel;

            PushRowElement(scrollViewer);
        }

        public async override void OnTabSelected()
        {
            if (m_HasLoaded)
                return;

            m_HasLoaded = true;

            byte[]? fileContent = null;
            if (File.Exists(SUBMISSIONS_CACHED_FILENAME))
                try
                {
                    fileContent = File.ReadAllBytes(SUBMISSIONS_CACHED_FILENAME);
                }
                catch (IOException)
                {

                }

            if (fileContent != null)
                try
                {
                    await ParseWorkshopSubmissionsFileAsync(fileContent);
                }
                catch (JsonException)
                {
                    await ForceRefreshWorkshopContentFromRemote();
                }
            else
                await ForceRefreshWorkshopContentFromRemote();
        }

        private void SortSubmissionItems()
        {
            ApplySortAndFilter();
        }
    }
}
