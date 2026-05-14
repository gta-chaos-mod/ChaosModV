using System.IO;
using System.Net.Http;
using ConfigApp.Infrastructure;
using ConfigApp.Tabs;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json.Linq;
using Windows.Graphics;
using WinRT.Interop;
using static ConfigApp.Effects;

namespace ConfigApp
{
    public partial class MainWindow : Window
    {
        private const string VersionUrl = "https://raw.githubusercontent.com/gta-chaos-mod/ChaosModV/refs/heads/master/version.txt";

        private readonly ITabLifecycle[] m_Tabs;

        private readonly List<TreeMenuItem> m_TreeMenuItemsAll = [];
        private readonly List<TreeMenuItem> m_TreeMenuItemsFiltered = [];
        private TreeMenuItem? m_MetaParentItem;
        private readonly Dictionary<string, EffectData> m_EffectDataMap = [];

        private bool m_StartupCompleted;

        public MainWindow()
        {
            InitializeComponent();
            InitializeWindow();

            m_Tabs = [settings_tab, voting_tab, workshop_tab, more_tab];

            Utils.AttachNumericTextBoxBehavior(meta_effects_spawn_dur);
            Utils.AttachNumericTextBoxBehavior(meta_effects_timed_dur);
            Utils.AttachNumericTextBoxBehavior(meta_effects_short_timed_dur);

            Title = $"ChaosModV Configuration (v{Info.VERSION})";
            user_save.Style = (Style)Application.Current.Resources["AccentButtonStyle"];
            MainRoot.Loaded += OnLoaded;

            ShowSection("Effects");
        }

        private void InitializeWindow()
        {
            var hwnd = WindowNative.GetWindowHandle(this);
            var windowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(hwnd);
            var appWindow = AppWindow.GetFromWindowId(windowId);
            appWindow.Resize(new SizeInt32(1200, 780));

            if (AppWindowTitleBar.IsCustomizationSupported())
            {
                ExtendsContentIntoTitleBar = true;
                SetTitleBar(AppTitleBar);
            }
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (m_StartupCompleted)
                return;

            m_StartupCompleted = true;
            await InitializeAsync();
        }

        private async Task InitializeAsync()
        {
            await CheckForUpdatesAsync();
            OptionsManager.ReadFiles();

            foreach (var tab in m_Tabs)
                tab.OnLoadValues();

            ParseConfigFile();
            ParseEffectsFile();
            InitEffectsTreeView();

            try
            {
                VerifyWriteAccess();
            }
            catch (Exception ex) when (ex is UnauthorizedAccessException or FileNotFoundException)
            {
                await AppDialog.ShowMessageAsync("No permissions to write in the current directory. Try to either run the program with admin privileges or allow write access to the current directory.", "No Write Access");
                Application.Current.Exit();
            }
        }

        private static void VerifyWriteAccess()
        {
            const string testFile = ".writetest";
            if (!File.Exists(testFile))
            {
                using (File.Create(testFile))
                {
                }

                File.Delete(testFile);
            }
        }

        private void OnNavigationSelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs eventArgs)
        {
            if (eventArgs.SelectedItem is not NavigationViewItem selectedItem || selectedItem.Tag is not string selectedTag)
                return;

            switch (selectedTag)
            {
            case "Settings":
                ShowSection("Settings");
                settings_tab.SelectSubSection("General");
                break;
            case "Settings:General":
                ShowSection("Settings");
                settings_tab.SelectSubSection("General");
                break;
            case "Settings:Modes":
                ShowSection("Settings");
                settings_tab.SelectSubSection("Modes");
                break;
            case "Settings:Shortcuts":
                ShowSection("Settings");
                settings_tab.SelectSubSection("Shortcuts");
                break;
            case "Settings:Colors":
                ShowSection("Settings");
                settings_tab.SelectSubSection("Colors");
                break;
            case "Settings:Sounds":
                ShowSection("Settings");
                settings_tab.SelectSubSection("Sounds");
                break;
            case "Voting":
                ShowSection("Voting");
                voting_tab.SelectSubSection("General");
                break;
            case "Voting:General":
                ShowSection("Voting");
                voting_tab.SelectSubSection("General");
                break;
            case "Voting:Twitch":
                ShowSection("Voting");
                voting_tab.SelectSubSection("Twitch");
                break;
            case "Voting:Discord":
                ShowSection("Voting");
                voting_tab.SelectSubSection("Discord");
                break;
            case "Workshop":
                ShowSection("Workshop");
                workshop_tab.OnTabSelected();
                break;
            case "More":
                ShowSection("More");
                more_tab.OnTabSelected();
                break;
            default:
                ShowSection(selectedTag);
                break;
            }
        }

        private void ShowSection(string section)
        {
            EffectsSection.Visibility = section == "Effects" ? Visibility.Visible : Visibility.Collapsed;
            MetaSection.Visibility = section == "Meta" ? Visibility.Visible : Visibility.Collapsed;
            settings_tab.Visibility = section == "Settings" ? Visibility.Visible : Visibility.Collapsed;
            voting_tab.Visibility = section == "Voting" ? Visibility.Visible : Visibility.Collapsed;
            workshop_tab.Visibility = section == "Workshop" ? Visibility.Visible : Visibility.Collapsed;
            more_tab.Visibility = section == "More" ? Visibility.Visible : Visibility.Collapsed;
        }

        private async Task CheckForUpdatesAsync()
        {
            using var httpClient = new HttpClient();

            try
            {
                var newVersion = (await httpClient.GetStringAsync(VersionUrl)).Trim();
                var isUpdateAvailable = Info.VERSION != newVersion;
                update_available_label.Text = isUpdateAvailable ? $"Update available: v{newVersion}" : "You are running the latest version.";
                update_available_button.Visibility = isUpdateAvailable ? Visibility.Visible : Visibility.Collapsed;
            }
            catch (HttpRequestException)
            {
                update_available_label.Text = "Could not check for updates.";
            }
        }

        private EffectData GetOrCreateEffectData(string effectId)
        {
            if (m_EffectDataMap.TryGetValue(effectId, out var effectData))
                return effectData;

            effectData = new EffectData();
            m_EffectDataMap.Add(effectId, effectData);
            return effectData;
        }

        private void ParseConfigFile()
        {
            meta_effects_spawn_dur.Text = OptionsManager.ConfigFile.ReadValue("NewMetaEffectSpawnTime", 600).ToString();
            meta_effects_timed_dur.Text = OptionsManager.ConfigFile.ReadValue("MetaEffectDur", 95).ToString();
            meta_effects_short_timed_dur.Text = OptionsManager.ConfigFile.ReadValue("MetaShortEffectDur", 65).ToString();
        }

        private void WriteConfigFile()
        {
            OptionsManager.ConfigFile.WriteValueAsInt("NewMetaEffectSpawnTime", meta_effects_spawn_dur.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaEffectDur", meta_effects_timed_dur.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaShortEffectDur", meta_effects_short_timed_dur.Text);
        }

        private void ParseEffectsFile()
        {
            var isJson = OptionsManager.EffectsFile.FoundFilePath.EndsWith(".json");
            foreach (var key in OptionsManager.EffectsFile.GetKeys())
            {
                var effectData = isJson
                    ? Utils.ValueObjectToEffectData(OptionsManager.EffectsFile.ReadValue<JObject>(key))
                    : Utils.ValueStringToEffectData(OptionsManager.EffectsFile.ReadValue<string>(key));

                m_EffectDataMap[key] = effectData;
            }
        }

        private void WriteEffectsFile()
        {
            OptionsManager.EffectsFile.ResetFile();

            foreach (var (effectId, _) in EffectsMap)
                OptionsManager.EffectsFile.WriteValue(effectId, BuildEffectJson(effectId));

            OptionsManager.EffectsFile.WriteFile();
        }

        private JObject BuildEffectJson(string effectId)
        {
            var data = GetOrCreateEffectData(effectId);
            var json = new JObject();

            if (data.Enabled is not null)
                json["enabled"] = new JValue(data.Enabled.Value);

            if (data.CustomTime is not null)
                json["customTime"] = new JValue(data.CustomTime.Value);

            if (data.ExcludedFromVoting is not null)
                json["excludedFromVoting"] = new JValue(data.ExcludedFromVoting.Value);

            if (data.TimedType is not null)
            {
                json["permanent"] = data.TimedType == EffectTimedType.Permanent;
                json["timedType"] = (int)data.TimedType;
            }

            if (data.ShortcutKeycode is not null)
                json["shortcutKeycode"] = new JValue(data.ShortcutKeycode.Value);

            if (data.WeightMult is not null)
                json["weightMult"] = new JValue(data.WeightMult.Value);

            if (data.CustomName is not null)
                json["customName"] = new JValue(data.CustomName);

            return json;
        }

        private void InitEffectsTreeView()
        {
            effects_user_effects_search.Text = string.Empty;

            m_TreeMenuItemsAll.Clear();

            var categoryMap = CreateEffectCategoryMap();

            foreach (var pair in GetSortedEffects())
            {
                var effectName = pair.Key;
                var effectId = pair.Value.EffectId;
                var effectCategory = pair.Value.EffectCategory;
                var effectData = GetOrCreateEffectData(effectId);

                var menuItem = CreateEffectMenuItem(effectName, effectId, effectData);
                categoryMap[effectCategory].AddChild(menuItem);
            }

            m_TreeMenuItemsAll.AddRange(categoryMap.Values);
            m_MetaParentItem = categoryMap[EffectCategory.Meta];
            m_TreeMenuItemsFiltered.Clear();
            m_TreeMenuItemsFiltered.AddRange(m_TreeMenuItemsAll);

            foreach (var treeMenuItem in m_TreeMenuItemsAll)
                treeMenuItem.UpdateCheckedAccordingToChildrenStatus();

            RefreshEffectsTrees();
        }

        private static Dictionary<EffectCategory, TreeMenuItem> CreateEffectCategoryMap()
        {
            return new Dictionary<EffectCategory, TreeMenuItem>
            {
                [EffectCategory.Player] = new TreeMenuItem("Player"),
                [EffectCategory.Vehicle] = new TreeMenuItem("Vehicle"),
                [EffectCategory.Peds] = new TreeMenuItem("Peds"),
                [EffectCategory.Screen] = new TreeMenuItem("Screen"),
                [EffectCategory.Time] = new TreeMenuItem("Time"),
                [EffectCategory.Weather] = new TreeMenuItem("Weather"),
                [EffectCategory.Misc] = new TreeMenuItem("Misc"),
                [EffectCategory.Meta] = new TreeMenuItem("Meta")
            };
        }

        private static SortedDictionary<string, (string EffectId, EffectCategory EffectCategory)> GetSortedEffects()
        {
            var sortedEffects = new SortedDictionary<string, (string EffectId, EffectCategory EffectCategory)>();

            foreach (var pair in EffectsMap)
            {
                if (pair.Value.Name is not null)
                    sortedEffects.Add(pair.Value.Name, (pair.Key, pair.Value.EffectCategory));
            }

            return sortedEffects;
        }

        private static TreeMenuItem CreateEffectMenuItem(string effectName, string effectId, EffectData effectData)
        {
            var menuItem = new TreeMenuItem(effectName);
            menuItem.OnConfigureClickAsync = async () =>
            {
                var effectInfo = EffectsMap[effectId];
                var effectConfig = new EffectConfigDialog(effectId, effectData, effectInfo);
                await effectConfig.ShowAsync();

                if (!effectConfig.IsSaved)
                    return;

                effectConfig.GetNewData();
                menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
            };
            menuItem.OnCheckedClick = () => effectData.Enabled = menuItem.IsChecked;
            menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
            menuItem.IsChecked = effectData.Enabled ?? true;
            return menuItem;
        }

        private void RefreshEffectsTrees()
        {
            TreeViewBuilder.Populate(effects_user_effects_tree_view, m_TreeMenuItemsFiltered, RefreshEffectsTrees);

            if (m_MetaParentItem is not null)
                TreeViewBuilder.Populate(meta_effects_tree_view, [m_MetaParentItem], RefreshEffectsTrees);
        }

        private void OnUserEffectSearchTextChanged(object sender, TextChangedEventArgs e)
        {
            FilterEffectsBySearchText(effects_user_effects_search.Text?.Trim());
        }

        private void FilterEffectsBySearchText(string? filterText)
        {
            m_TreeMenuItemsFiltered.Clear();

            if (string.IsNullOrWhiteSpace(filterText))
            {
                m_TreeMenuItemsFiltered.AddRange(m_TreeMenuItemsAll);
            }
            else
            {
                foreach (var parentItem in m_TreeMenuItemsAll)
                {
                    foreach (var childItem in parentItem.Children)
                    {
                        if (childItem.Text.Contains(filterText, StringComparison.InvariantCultureIgnoreCase))
                            m_TreeMenuItemsFiltered.Add(childItem);
                    }
                }
            }

            RefreshEffectsTrees();
        }

        private void SetSaveControlsEnabled(bool isEnabled)
        {
            user_save.IsEnabled = isEnabled;
            user_reset.IsEnabled = isEnabled;
        }

        private static bool IsLegacyConfigInRoot()
        {
            return OptionsManager.ConfigFile.FoundFilePath == "config.ini"
                || OptionsManager.VotingFile.FoundFilePath == "twitch.ini"
                || OptionsManager.EffectsFile.FoundFilePath == "effects.ini";
        }

        private static bool IsLegacyConfigFormatPresent()
        {
            return IsLegacyConfigInRoot()
                || OptionsManager.ConfigFile.FoundFilePath == "configs/config.ini"
                || OptionsManager.VotingFile.FoundFilePath == "configs/twitch.ini"
                || OptionsManager.VotingFile.FoundFilePath == "configs/voting.ini"
                || OptionsManager.EffectsFile.FoundFilePath == "configs/effects.ini";
        }

        private static async Task<bool> TryHandleConfigMigrationWarningsAsync()
        {
            if (IsLegacyConfigInRoot())
            {
                if (!await AppDialog.ShowOkCancelAsync("Config files reside inside the configs/ subdirectory now. Clicking OK will move the config files there. If you want to play older versions of the mod you will have to move them back. Continue?"))
                    return false;
            }

            if (IsLegacyConfigFormatPresent())
            {
                if (!await AppDialog.ShowOkCancelAsync("WARNING: Starting with mod version 2.2 config files are automatically migrated to the new JSON format. Clicking OK will migrate your config files. This will prevent you from using earlier mod versions with your existing config. Your old config files will be backed up to the configs/old/ directory. Continue?"))
                    return false;

                BackupLegacyConfigFiles();
            }

            return true;
        }

        private static void BackupLegacyConfigFiles()
        {
            const string backupDir = "configs/old";
            Directory.CreateDirectory(backupDir);

            MoveFileToBackup(OptionsManager.ConfigFile.FoundFilePath, backupDir);
            MoveFileToBackup(OptionsManager.VotingFile.FoundFilePath, backupDir);
            MoveFileToBackup(OptionsManager.EffectsFile.FoundFilePath, backupDir);
        }

        private static void MoveFileToBackup(string filePath, string backupDirectory)
        {
            var fileName = Path.GetFileName(filePath);
            File.Move(filePath, Path.Combine(backupDirectory, fileName), true);
        }

        private async void OnUserSaveClick(object sender, RoutedEventArgs e)
        {
            SetSaveControlsEnabled(false);

            try
            {
                if (!await TryHandleConfigMigrationWarningsAsync())
                    return;

                WriteConfigFile();
                WriteEffectsFile();

                foreach (var tab in m_Tabs)
                    tab.OnSaveValues();

                OptionsManager.WriteFiles();

                foreach (var tab in m_Tabs)
                    tab.OnLoadValues();

                OptionsManager.DeleteCompatFiles();

                await AppDialog.ShowMessageAsync("Saved config!\nMake sure to press CTRL + L in-game twice if mod is already running to reload the config.");
            }
            finally
            {
                SetSaveControlsEnabled(true);
            }
        }

        private async void OnUserResetClick(object sender, RoutedEventArgs e)
        {
            if (!await AppDialog.ShowYesNoAsync("Are you sure you want to reset your config?"))
                return;

            OptionsManager.ResetFiles();

            if (await AppDialog.ShowYesNoAsync("Do you want to reset your voting settings too?"))
                OptionsManager.VotingFile.ResetFile();

            await InitializeAsync();
            await AppDialog.ShowMessageAsync("Config has been reverted to default settings!");
        }

        private void OpenModPageEvent(object sender, RoutedEventArgs eventArgs)
        {
            Utils.OpenUrl("https://www.gta5-mods.com/scripts/chaos-mod-v");
        }
    }
}
