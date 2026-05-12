using System.IO;
using System.Net.Http;
using ConfigApp.Infrastructure;
using ConfigApp.Tabs;
using ConfigApp.Tabs.Settings;
using ConfigApp.Tabs.Voting;
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
        private readonly Dictionary<string, Tab> m_Tabs = new()
        {
            { "Settings", new SettingsTab() },
            { "Voting", new VotingTab() },
            { "Workshop", new WorkshopTab() },
            { "More", new MoreTab() }
        };

        private Dictionary<string, TreeMenuItem>? m_TreeMenuItemsMap = null;
        private List<TreeMenuItem>? m_TreeMenuItemsAll = null;
        private List<TreeMenuItem>? m_TreeMenuItemsFiltered = null;
        private TreeMenuItem? m_MetaParentItem = null;

        private Dictionary<string, EffectData>? m_EffectDataMap = null;

        private bool m_InitializedTabs = false;
        private bool m_StartupCompleted = false;

        public MainWindow()
        {
            InitializeComponent();
            InitializeWindow();
            InitializeTabs();

            Utils.AttachNumericTextBoxBehavior(meta_effects_spawn_dur);
            Utils.AttachNumericTextBoxBehavior(meta_effects_timed_dur);
            Utils.AttachNumericTextBoxBehavior(meta_effects_short_timed_dur);

            Title = $"ChaosModV Configuration (v{Info.VERSION})";
            user_save.Style = (Style)Application.Current.Resources["AccentButtonStyle"];
            MainRoot.Loaded += OnLoaded;
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

        private void InitializeTabs()
        {
            if (m_InitializedTabs)
                return;

            m_InitializedTabs = true;

            foreach (var tab in m_Tabs)
            {
                var tabItem = new TabViewItem
                {
                    Header = tab.Key
                };

                var grid = new Grid();
                tab.Value.Init(grid);
                tabItem.Content = grid;
                root_tabcontrol.TabItems.Add(tabItem);
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

            foreach (var tab in m_Tabs.Values)
                tab.OnLoadValues();

            m_EffectDataMap = new Dictionary<string, EffectData>();

            ParseConfigFile();
            ParseEffectsFile();
            InitEffectsTreeView();

            try
            {
                if (!File.Exists(".writetest"))
                {
                    using (File.Create(".writetest"))
                    {
                    }

                    File.Delete(".writetest");
                }
            }
            catch (Exception e) when (e is UnauthorizedAccessException || e is FileNotFoundException)
            {
                await AppDialog.ShowMessageAsync("No permissions to write in the current directory. Try to either run the program with admin privileges or allow write access to the current directory.", "No Write Access");
                Application.Current.Exit();
            }
        }

        private void OnTabSelectionChanged(object sender, SelectionChangedEventArgs eventArgs)
        {
            if (root_tabcontrol.SelectedItem is not TabViewItem selectedTab || selectedTab.Header is not string selectedHeader)
                return;

            if (m_Tabs.TryGetValue(selectedHeader, out var tab))
                tab.OnTabSelected();
        }

        private async Task CheckForUpdatesAsync()
        {
            using var httpClient = new HttpClient();

            try
            {
                var newVersion = (await httpClient.GetStringAsync("https://raw.githubusercontent.com/gta-chaos-mod/ChaosModV/refs/heads/master/version.txt")).Trim();
                update_available_label.Text = Info.VERSION != newVersion ? $"Update available: v{newVersion}" : "You are running the latest version.";
                update_available_button.Visibility = Info.VERSION != newVersion ? Visibility.Visible : Visibility.Collapsed;
            }
            catch (HttpRequestException)
            {
                update_available_label.Text = "Could not check for updates.";
            }
        }

        private EffectData GetEffectData(string effectId)
        {
            if (m_EffectDataMap?.TryGetValue(effectId, out EffectData? effectData) is not true)
            {
                effectData = new EffectData();
                m_EffectDataMap?.Add(effectId, effectData);
            }

            return effectData;
        }

        private void ParseConfigFile()
        {
            meta_effects_spawn_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("NewMetaEffectSpawnTime", 600)}";
            meta_effects_timed_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaEffectDur", 95)}";
            meta_effects_short_timed_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaShortEffectDur", 65)}";
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

                m_EffectDataMap?.Add(key, effectData);
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
            var effectData = GetEffectData(effectId);
            var json = new JObject();

            if (effectData.Enabled is not null)
                json["enabled"] = effectData.Enabled;
            if (effectData.CustomTime is not null)
                json["customTime"] = effectData.CustomTime;
            if (effectData.ExcludedFromVoting is not null)
                json["excludedFromVoting"] = effectData.ExcludedFromVoting;
            if (effectData.TimedType is not null)
                json["permanent"] = effectData.TimedType == EffectTimedType.Permanent;
            if (effectData.ShortcutKeycode is not null)
                json["shortcutKeycode"] = effectData.ShortcutKeycode;
            if (effectData.TimedType is not null)
                json["timedType"] = (int)effectData.TimedType;
            if (effectData.WeightMult is not null)
                json["weightMult"] = effectData.WeightMult;
            if (effectData.CustomName is not null)
                json["customName"] = effectData.CustomName;

            return json;
        }

        private void InitEffectsTreeView()
        {
            effects_user_effects_search.Text = string.Empty;

            m_TreeMenuItemsMap = new Dictionary<string, TreeMenuItem>();
            m_TreeMenuItemsAll = new List<TreeMenuItem>();

            var categoryMap = CreateEffectCategoryMap();

            foreach (var pair in GetSortedEffects())
            {
                var effectName = pair.Key;
                var effectId = pair.Value.EffectId;
                var effectCategory = pair.Value.EffectCategory;
                var effectData = GetEffectData(effectId);

                var menuItem = CreateEffectMenuItem(effectName, effectId, effectData);
                categoryMap[effectCategory].AddChild(menuItem);
                m_TreeMenuItemsMap.Add(effectId, menuItem);
            }

            m_TreeMenuItemsAll.AddRange(categoryMap.Values);
            m_MetaParentItem = categoryMap[EffectCategory.Meta];
            m_TreeMenuItemsFiltered = m_TreeMenuItemsAll.ToList();

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
                if (pair.Value.Name is null)
                    continue;

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
                var effectConfig = new EffectConfig(effectId, effectData, effectInfo);
                await effectConfig.ShowAsync();

                if (!effectConfig.IsSaved)
                    return;

                effectData = effectConfig.GetNewData();
                if (effectData.TimedType == EffectTimedType.Permanent)
                    menuItem.IsColored = true;
            };
            menuItem.OnCheckedClick = () => effectData.Enabled = menuItem.IsChecked;
            menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
            menuItem.IsChecked = effectData.Enabled ?? true;
            return menuItem;
        }

        private void RefreshEffectsTrees()
        {
            if (m_TreeMenuItemsFiltered is not null)
                TreeViewBuilder.Populate(effects_user_effects_tree_view, m_TreeMenuItemsFiltered, RefreshEffectsTrees);

            if (m_MetaParentItem is not null)
                TreeViewBuilder.Populate(meta_effects_tree_view, new[] { m_MetaParentItem }, RefreshEffectsTrees);
        }

        private void OnUserEffectSearchTextChanged(object sender, TextChangedEventArgs e)
        {
            var filterText = effects_user_effects_search.Text?.Trim();

            if (m_TreeMenuItemsAll is not null && m_TreeMenuItemsFiltered is not null)
            {
                m_TreeMenuItemsFiltered.Clear();
                foreach (var parentMenuItem in m_TreeMenuItemsAll)
                {
                    if (string.IsNullOrWhiteSpace(filterText))
                    {
                        m_TreeMenuItemsFiltered.Add(parentMenuItem);
                        continue;
                    }

                    if (parentMenuItem.Children == null)
                        continue;

                    foreach (var childMenuItem in parentMenuItem.Children)
                    {
                        if (childMenuItem.Text.Contains(filterText, StringComparison.InvariantCultureIgnoreCase))
                            m_TreeMenuItemsFiltered.Add(childMenuItem);
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

        private bool IsLegacyConfigInRoot()
        {
            return OptionsManager.ConfigFile.FoundFilePath == "config.ini"
                || OptionsManager.VotingFile.FoundFilePath == "twitch.ini"
                || OptionsManager.EffectsFile.FoundFilePath == "effects.ini";
        }

        private bool IsLegacyConfigFormatPresent()
        {
            return IsLegacyConfigInRoot()
                || OptionsManager.ConfigFile.FoundFilePath == "configs/config.ini"
                || OptionsManager.VotingFile.FoundFilePath == "configs/twitch.ini"
                || OptionsManager.VotingFile.FoundFilePath == "configs/voting.ini"
                || OptionsManager.EffectsFile.FoundFilePath == "configs/effects.ini";
        }

        private async Task<bool> TryHandleConfigMigrationWarningsAsync()
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

                Directory.CreateDirectory("configs/old");
                File.Move(OptionsManager.ConfigFile.FoundFilePath, $"configs/old/{Path.GetFileName(OptionsManager.ConfigFile.FoundFilePath)}", true);
                File.Move(OptionsManager.VotingFile.FoundFilePath, $"configs/old/{Path.GetFileName(OptionsManager.VotingFile.FoundFilePath)}", true);
                File.Move(OptionsManager.EffectsFile.FoundFilePath, $"configs/old/{Path.GetFileName(OptionsManager.EffectsFile.FoundFilePath)}", true);
            }

            return true;
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

                foreach (var tab in m_Tabs.Values)
                    tab.OnSaveValues();

                OptionsManager.WriteFiles();

                foreach (var tab in m_Tabs.Values)
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
            if (await AppDialog.ShowYesNoAsync("Are you sure you want to reset your config?"))
            {
                OptionsManager.ResetFiles();

                if (await AppDialog.ShowYesNoAsync("Do you want to reset your voting settings too?"))
                    OptionsManager.VotingFile.ResetFile();

                await InitializeAsync();

                await AppDialog.ShowMessageAsync("Config has been reverted to default settings!");
            }
        }

        public void OpenModPageEvent(object sender, RoutedEventArgs eventArgs)
        {
            Utils.OpenURL("https://www.gta5-mods.com/scripts/chaos-mod-v");
        }
    }
}
