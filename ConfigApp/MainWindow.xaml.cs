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
        private readonly Dictionary<string, TabViewItem> m_TabItems = new();

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
            if (!m_InitializedTabs)
            {
                m_InitializedTabs = true;

                foreach (var tab in m_Tabs)
                {
                    var tabItem = new TabViewItem()
                    {
                        Header = tab.Key,
                    };

                    var grid = new Grid();

                    tab.Value.Init(grid);

                    tabItem.Content = grid;

                    root_tabcontrol.TabItems.Add(tabItem);

                    m_TabItems[tab.Key] = tabItem;
                }
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
            CheckForUpdates();

            OptionsManager.ReadFiles();

            foreach (var tab in m_Tabs)
                tab.Value.OnLoadValues();

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

        private async void CheckForUpdates()
        {
            var httpClient = new HttpClient();

            try
            {
                string newVersion = (await httpClient.GetStringAsync("https://raw.githubusercontent.com/gta-chaos-mod/ChaosModV/refs/heads/master/version.txt")).Trim();

                if (Info.VERSION != newVersion)
                {
                    update_available_label.Text = $"Update available: v{newVersion}";
                    update_available_button.Visibility = Visibility.Visible;
                }
                else
                {
                    update_available_label.Text = "You are running the latest version.";
                }
            }
            catch (HttpRequestException)
            {
                update_available_label.Text = "Could not check for updates.";
            }
        }

        private EffectData GetEffectData(string effectId)
        {
            // Create EffectData in case effect wasn't saved yet
            if (m_EffectDataMap?.TryGetValue(effectId, out EffectData? effectData) is not true)
            {
                effectData = new EffectData();
                m_EffectDataMap?.Add(effectId, effectData);
            }

            return effectData;
        }

        private void ParseConfigFile()
        {
            // Meta Effects
            meta_effects_spawn_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("NewMetaEffectSpawnTime", 600)}";
            meta_effects_timed_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaEffectDur", 95)}";
            meta_effects_short_timed_dur.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaShortEffectDur", 65)}";
        }

        private void WriteConfigFile()
        {
            // Meta Effects
            OptionsManager.ConfigFile.WriteValueAsInt("NewMetaEffectSpawnTime", meta_effects_spawn_dur.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaEffectDur", meta_effects_timed_dur.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaShortEffectDur", meta_effects_short_timed_dur.Text);
        }

        private void ParseEffectsFile()
        {
            bool isJson = OptionsManager.EffectsFile.FoundFilePath.EndsWith(".json");
            foreach (string key in OptionsManager.EffectsFile.GetKeys())
            {
                EffectData effectData;
                if (isJson)
                    effectData = Utils.ValueObjectToEffectData(OptionsManager.EffectsFile.ReadValue<JObject>(key));
                else
                    effectData = Utils.ValueStringToEffectData(OptionsManager.EffectsFile.ReadValue<string>(key));

                m_EffectDataMap?.Add(key, effectData);
            }
        }

        private void WriteEffectsFile()
        {
            OptionsManager.EffectsFile.ResetFile();

            foreach (var (effectId, _) in EffectsMap)
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

                OptionsManager.EffectsFile.WriteValue(effectId, json);
            }

            OptionsManager.EffectsFile.WriteFile();
        }

        private void InitEffectsTreeView()
        {
            effects_user_effects_search.Text = string.Empty;

            m_TreeMenuItemsMap = new Dictionary<string, TreeMenuItem>();
            m_TreeMenuItemsAll = new List<TreeMenuItem>();

            var playerParentItem = new TreeMenuItem("Player");
            var vehicleParentItem = new TreeMenuItem("Vehicle");
            var pedsParentItem = new TreeMenuItem("Peds");
            var screenParentItem = new TreeMenuItem("Screen");
            var timeParentItem = new TreeMenuItem("Time");
            var weatherParentItem = new TreeMenuItem("Weather");
            var miscParentItem = new TreeMenuItem("Misc");
            var metaParentItem = new TreeMenuItem("Meta");

            var sortedEffects = new SortedDictionary<string, (string EffectId, EffectCategory EffectCategory)>();

            foreach (var pair in EffectsMap)
            {
                if (pair.Value.Name is null)
                    continue;

                sortedEffects.Add(pair.Value.Name, (EffectId: pair.Key, pair.Value.EffectCategory));
            }

            foreach (var effect in sortedEffects)
            {
                var effectName = effect.Key;
                var effectMisc = effect.Value;
                var effectData = GetEffectData(effectMisc.EffectId);

                var menuItem = new TreeMenuItem(effectName);
                menuItem.OnConfigureClickAsync = async () =>
                {
                    var effectInfo = EffectsMap[effectMisc.EffectId];

                    var effectConfig = new EffectConfig(effectMisc.EffectId, effectData, effectInfo);
                    await effectConfig.ShowAsync();

                    if (!effectConfig.IsSaved)
                        return;

                    effectData = effectConfig.GetNewData();
                    if (m_EffectDataMap is not null)
                        m_EffectDataMap[effectMisc.EffectId] = effectData;
                    menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
                };
                menuItem.OnCheckedClick = () =>
                {
                    effectData.Enabled = menuItem.IsChecked;
                };
                menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
                menuItem.IsChecked = effectData.Enabled ?? true;
                m_TreeMenuItemsMap.Add(effectMisc.EffectId, menuItem);

                switch (effectMisc.EffectCategory)
                {
                case EffectCategory.Player:
                    playerParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Vehicle:
                    vehicleParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Peds:
                    pedsParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Screen:
                    screenParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Time:
                    timeParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Weather:
                    weatherParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Misc:
                    miscParentItem.AddChild(menuItem);
                    break;
                case EffectCategory.Meta:
                    metaParentItem.AddChild(menuItem);
                    break;
                }
            }

            m_TreeMenuItemsAll.Add(playerParentItem);
            m_TreeMenuItemsAll.Add(vehicleParentItem);
            m_TreeMenuItemsAll.Add(pedsParentItem);
            m_TreeMenuItemsAll.Add(screenParentItem);
            m_TreeMenuItemsAll.Add(timeParentItem);
            m_TreeMenuItemsAll.Add(weatherParentItem);
            m_TreeMenuItemsAll.Add(miscParentItem);
            m_MetaParentItem = metaParentItem;

            m_TreeMenuItemsFiltered = m_TreeMenuItemsAll.ToList();

            foreach (var treeMenuItem in m_TreeMenuItemsAll.Append(metaParentItem))
                treeMenuItem.UpdateCheckedAccordingToChildrenStatus();

            RefreshEffectsTrees();
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
