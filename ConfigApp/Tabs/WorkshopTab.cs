using System.Collections.ObjectModel;
using System.IO;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Markup;
using System.Windows.Media;
using ConfigApp.Workshop;
using Microsoft.CSharp.RuntimeBinder;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Xceed.Wpf.Toolkit;
using ZstdSharp;
using MessageBox = System.Windows.MessageBox;

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

        private ObservableCollection<WorkshopSubmissionItem> m_WorkshopSubmissionItems = new();

        private CheckBox? m_SortIntalledFirstToggle = null;
        private WatermarkTextBox? m_SearchBox = null;
        private ItemsControl? m_ItemsControl = null;

        private void SortSubmissionItems()
        {
            IOrderedEnumerable<WorkshopSubmissionItem>? items = null;

            items = m_SortingMode switch
            {
                SortingMode.Name => m_WorkshopSubmissionItems.OrderBy(item => item.Name?.ToLower()),
                SortingMode.LastUpdated => m_WorkshopSubmissionItems.OrderByDescending(item => item.LastUpdated),
                SortingMode.Author => m_WorkshopSubmissionItems.OrderBy(item => item.Author?.ToLower()),
                _ => throw new NotImplementedException(),
            };
            if (m_SortIntalledFirstToggle == null || m_SortIntalledFirstToggle.IsChecked.GetValueOrDefault(true))
                items = items.OrderBy(item => item.InstallState);

            m_WorkshopSubmissionItems = new ObservableCollection<WorkshopSubmissionItem>(items);
            if (m_ItemsControl is not null)
                m_ItemsControl.ItemsSource = m_WorkshopSubmissionItems;
        }

        private void HandleWorkshopSubmissionsSearchFilter()
        {
            var transformedText = m_SearchBox?.Text.Trim().ToLower();
            var view = CollectionViewSource.GetDefaultView(m_WorkshopSubmissionItems);
            view.Filter = (submissionItem) =>
            {
                if (submissionItem is not WorkshopSubmissionItem item || transformedText is null || transformedText == "")
                    return true;

                foreach (var term in item.SearchTerms)
                {
                    if (term.Term.ToLower().Contains(transformedText))
                        return true;
                }

                return false;
            };

            foreach (var item in m_WorkshopSubmissionItems)
            {
                item.HighlightedFiles.Clear();
                if (transformedText is not null && transformedText != "")
                {
                    foreach (var term in item.SearchTerms)
                    {
                        if (term.Term.ToLower().Contains(transformedText))
                        {
                            if (term.IsInFile)
                                item.HighlightedFiles.Add(term.FileName);
                        }
                    }
                }
            }
        }

        private void ParseWorkshopSubmissionsFile(byte[] compressedFileContent)
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
                var id = directory.Split('/')[1];

                if (!File.Exists($"{directory}/metadata.json"))
                {
                    MessageBox.Show($"Local submission \"{id}\" is missing a metadata.json.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Warning);
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
                    MessageBox.Show($"Local submission \"{id}\" has a corrupt metadata.json.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Warning);
                    continue;
                }
            }

            SortSubmissionItems();

            HandleWorkshopSubmissionsSearchFilter();
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
                            ParseWorkshopSubmissionsFile(localContent);
                            return;
                        }
                    }
                }

                var submissionsResult = await httpClient.GetAsync($"{domain}/workshop/fetch_submissions");
                if (!submissionsResult.IsSuccessStatusCode)
                    MessageBox.Show("Remote server provided no master submissions file! Can not fetch available submissions.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                else
                {
                    var submissionsCompressedResult = await submissionsResult.Content.ReadAsByteArrayAsync();

                    ParseWorkshopSubmissionsFile(submissionsCompressedResult);

                    // Cache submissions
                    File.WriteAllBytes(SUBMISSIONS_CACHED_FILENAME, submissionsCompressedResult);
                }
            }
            catch (HttpRequestException)
            {
                MessageBox.Show("Error occured while trying to fetch submissions from server! Please try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch (InvalidOperationException)
            {
                MessageBox.Show($"Specified workshop URL ({domain}) is invalid!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch (Exception exception) when (exception is JsonException || exception is ZstdException)
            {
                MessageBox.Show("Remote server provided a malformed master submissions file! Can not fetch available submissions.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void OnSettingsClick(object sender, RoutedEventArgs eventArgs)
        {
            var dialog = new WorkshopSettingsDialog();
            dialog.ShowDialog();
            if (dialog.IsSaved)
                await ForceRefreshWorkshopContentFromRemote();
        }

        private async void OnRefreshClick(object sender, RoutedEventArgs eventArgs)
        {
            var button = (Button)sender;

            button.IsEnabled = false;
            foreach (var item in m_WorkshopSubmissionItems)
            {
                item.Refresh();
            }
            await ForceRefreshWorkshopContentFromRemote();
            button.IsEnabled = true;
        }

        private void OnTextChangeSearch(object sender, TextChangedEventArgs eventArgs)
        {
            HandleWorkshopSubmissionsSearchFilter();
        }

        private void OnSortingModeBoxSelectionChanged(object sender, SelectionChangedEventArgs eventArgs)
        {
            var box = (ComboBox)sender;

            m_SortingMode = (SortingMode)box.SelectedIndex;
            SortSubmissionItems();
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));

            SetRowHeight(new GridLength());

            var headerGrid = new Grid();

            var sortingModeText = new TextBlock()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                Margin = new Thickness(2f, 3f, 0f, 0f),
                Text = "Sort By:"
            };
            headerGrid.Children.Add(sortingModeText);
            var sortingModeBox = new ComboBox()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                Width = 100f,
                Margin = new Thickness(55f, 0f, 0f, 0f),
                ItemsSource = m_SortingModeLabels.Values,
                SelectedIndex = 0
            };
            sortingModeBox.SelectionChanged += OnSortingModeBoxSelectionChanged;
            headerGrid.Children.Add(sortingModeBox);

            m_SortIntalledFirstToggle = new CheckBox()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Center,
                Margin = new Thickness(165f, 0f, 0f, 0f),
                IsChecked = true,
                Content = "Show installed first"
            };
            m_SortIntalledFirstToggle.Click += (sender, eventArgs) => { SortSubmissionItems(); };
            headerGrid.Children.Add(m_SortIntalledFirstToggle);

            m_SearchBox = new WatermarkTextBox()
            {
                HorizontalAlignment = HorizontalAlignment.Right,
                Width = 250f,
                Margin = new Thickness(0f, 0f, 70f, 0f),
                Watermark = "Search",
                KeepWatermarkOnGotFocus = true
            };
            m_SearchBox.TextChanged += OnTextChangeSearch;
            headerGrid.Children.Add(m_SearchBox);

            var settingsButton = new Button()
            {
                HorizontalAlignment = HorizontalAlignment.Right,
                Width = 25f,
                Margin = new Thickness(0f, 0f, 35f, 0f),
                ToolTip = "Settings",
                FontFamily = new FontFamily("Wingdings"),
                Content = new TextBlock()
                {
                    Text = "]",
                    FontSize = 19
                }
            };
            settingsButton.Click += OnSettingsClick;
            headerGrid.Children.Add(settingsButton);

            var refreshButton = new Button()
            {
                HorizontalAlignment = HorizontalAlignment.Right,
                Width = 25f,
                ToolTip = "Refresh",
                Content = new TextBlock()
                {
                    Text = "⟳",
                    FontSize = 19,
                    Margin = new Thickness(0f, -4f, 0f, 0f)
                }
            };
            refreshButton.Click += OnRefreshClick;
            headerGrid.Children.Add(refreshButton);

            PushRowElement(headerGrid);
            PopRow();

            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var scrollViewer = new ScrollViewer()
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch
            };

            m_ItemsControl = new ItemsControl()
            {
                ItemsPanel = (ItemsPanelTemplate)XamlReader.Parse(@"
                <ItemsPanelTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"" xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml"">
                    <WrapPanel Orientation=""Horizontal"" VerticalAlignment=""Top"" Width=""{Binding ElementName=MainGrid, Path=ActualWidth}""/>
                </ItemsPanelTemplate>
                "),
                ItemTemplate = (DataTemplate)XamlReader.Parse(@"
                <DataTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"" xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml"">
                    <Grid>
                        <Grid.RowDefinitions>
                            <RowDefinition Height=""40"" />
                            <RowDefinition Height=""*"" />
                            <RowDefinition Height=""40"" />
                        </Grid.RowDefinitions>

                        <Grid.ColumnDefinitions>
                            <ColumnDefinition Width=""50"" />
                            <ColumnDefinition Width=""340"" />
                            <ColumnDefinition />
                        </Grid.ColumnDefinitions>

                        <Image Grid.Row=""0"" Grid.Column=""0"" Width=""20"" Height=""20"" HorizontalAlignment=""Left"" Source=""{Binding SubmissionIcon}"" />
                        <TextBlock Grid.Row=""0"" Grid.Column=""1"" TextWrapping=""Wrap"" TextTrimming=""CharacterEllipsis"">
                            <TextBlock.Text>
                                <MultiBinding StringFormat=""{}{0}&#x0a;By {1}"">
                                    <Binding Path=""Name"" />
                                    <Binding Path=""Author"" />
                                </MultiBinding>
                            </TextBlock.Text>
                        </TextBlock>
                        <TextBlock Grid.Row=""1"" Grid.Column=""0"" TextWrapping=""Wrap"" Text=""{Binding Version}"" />
                        <TextBlock Grid.Row=""1"" Grid.Column=""1"" MinHeight=""70"" MaxHeight=""150"" TextWrapping=""Wrap"" TextTrimming=""CharacterEllipsis""
                            Text=""{Binding Description}"" />
                        <Button Grid.Row=""2"" Grid.Column=""1"" HorizontalAlignment=""Right"" VerticalAlignment=""Top"" Width=""30"" Height=""30""
                            Margin=""0,0,50,0"" Content=""]"" FontFamily=""Wingdings"" Visibility=""{Binding SettingsButtonVisibility}""
                            Command=""{Binding SettingsButtonCommand}"" />
                        <Button Grid.Row=""2"" Grid.Column=""1"" HorizontalAlignment=""Right"" VerticalAlignment=""Top"" Width=""30"" Height=""30""
                            Margin=""0,0,10,0"" Content=""i"" FontFamily=""Webdings"" Command=""{Binding InfoButtonCommand}"" />
                        <Button Grid.Row=""2"" Grid.Column=""2"" HorizontalAlignment=""Right"" VerticalAlignment=""Top"" Width=""60"" Height=""30""
                            Margin=""0,0,10,0"" Content=""{Binding InstallButtonText}"" IsEnabled=""{Binding InstallButtonEnabled}""
                            Command=""{Binding InstallButtonCommand}"" />
                    </Grid>
                </DataTemplate>
")
            };
            scrollViewer.Content = m_ItemsControl;

            PushRowElement(scrollViewer);
        }

        public async override void OnTabSelected()
        {
            // Only fetch them once
            if (m_WorkshopSubmissionItems.Count > 0)
                return;

            byte[]? fileContent = null;
            // Use cached content if existing (and accessible), otherwise fall back to server request
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
                    ParseWorkshopSubmissionsFile(fileContent);
                }
                catch (JsonException)
                {
                    // Cached file is corrupt, force reload
                    await ForceRefreshWorkshopContentFromRemote();
                }
            else
                await ForceRefreshWorkshopContentFromRemote();
        }
    }
}
