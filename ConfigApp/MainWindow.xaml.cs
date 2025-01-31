using System.IO;
using System.Net.Http;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using ConfigApp.Tabs;
using ConfigApp.Tabs.Settings;
using ConfigApp.Tabs.Voting;
using static ConfigApp.Effects;

namespace ConfigApp
{
    public partial class MainWindow : Window
    {
        private readonly Dictionary<string, Tab> m_Tabs = new()
        {
            //{ "Meta", new MetaTab() },
            { "Settings", new SettingsTab() },
            { "Voting", new VotingTab() },
            { "Workshop", new WorkshopTab() },
            { "More", new MoreTab() }
        };
        private readonly Dictionary<string, TabItem> m_TabItems = new();

        private bool m_bInitializedTitle = false;

        private Dictionary<string, TreeMenuItem>? m_TreeMenuItemsMap = null;
        private List<TreeMenuItem>? m_TreeMenuItemsAll = null;
        private List<TreeMenuItem>? m_TreeMenuItemsFiltered = null;

        private Dictionary<string, EffectData>? m_EffectDataMap = null;

        private bool m_InitializedTabs = false;

        public MainWindow()
        {
            Init();
        }

        private void Init()
        {
            InitializeComponent();

            if (!m_InitializedTabs)
            {
                m_InitializedTabs = true;

                foreach (var tab in m_Tabs)
                {
                    var tabItem = new TabItem()
                    {
                        Header = tab.Key,
                        Background = new SolidColorBrush(Color.FromRgb(0xF0, 0xF0, 0xF0)),
                        BorderBrush = new SolidColorBrush(Color.FromRgb(0xD3, 0xD3, 0xD3))
                    };

                    var grid = new Grid();

                    tab.Value.Init(grid);

                    tabItem.Content = grid;

                    root_tabcontrol.Items.Add(tabItem);

                    m_TabItems[tab.Key] = tabItem;
                }
            }

            if (!m_bInitializedTitle)
            {
                m_bInitializedTitle = true;

                Title += " (v" + Info.VERSION + ")";
            }

            CheckForUpdates();

            OptionsManager.ReadFiles();

            foreach (var tab in m_Tabs)
                tab.Value.OnLoadValues();

            m_EffectDataMap = new Dictionary<string, EffectData>();

            ParseConfigFile();

            ParseEffectsFile();

            InitEffectsTreeView();

            // Check write permissions
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
                MessageBox.Show("No permissions to write in the current directory. Try to either run the program with admin privileges or allow write access to the current directory.",
                    "No Write Access", MessageBoxButton.OK, MessageBoxImage.Error);

                Application.Current.Shutdown();
            }
        }

        private void OnTabSelectionChanged(object sender, SelectionChangedEventArgs eventArgs)
        {
            if (eventArgs.OriginalSource is not TabControl)
                return;

            foreach (var tab in m_TabItems)
            {
                if (tab.Value.IsSelected)
                {
                    m_Tabs[tab.Key].OnTabSelected();
                    break;
                }
            }
        }

        private async void CheckForUpdates()
        {
            var httpClient = new HttpClient();

            try
            {
                string newVersion = await httpClient.GetStringAsync("https://gopong.dev/chaos/version.txt");

                if (Info.VERSION != newVersion)
                    update_available_button.Visibility = Visibility.Visible;
                else
                    update_available_label.Text = "You are on the newest version of the mod!";
            }
            catch (HttpRequestException)
            {
                update_available_label.Text = "Unable to check for new updates!";
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
            meta_effects_spawn_dur.Text = OptionsManager.ConfigFile.ReadValue("NewMetaEffectSpawnTime", "600");
            meta_effects_timed_dur.Text = OptionsManager.ConfigFile.ReadValue("MetaEffectDur", "95");
            meta_effects_short_timed_dur.Text = OptionsManager.ConfigFile.ReadValue("MetaShortEffectDur", "65");
        }

        private void WriteConfigFile()
        {
            // Meta Effects
            OptionsManager.ConfigFile.WriteValue("NewMetaEffectSpawnTime", meta_effects_spawn_dur.Text);
            OptionsManager.ConfigFile.WriteValue("MetaEffectDur", meta_effects_timed_dur.Text);
            OptionsManager.ConfigFile.WriteValue("MetaShortEffectDur", meta_effects_short_timed_dur.Text);
        }

        private void ParseEffectsFile()
        {
            foreach (string key in OptionsManager.EffectsFile.GetKeys())
            {
                var value = OptionsManager.EffectsFile.ReadValue(key);
                var effectData = Utils.ValueStringToEffectData(value);

                m_EffectDataMap?.Add(key, effectData);
            }
        }

        private void WriteEffectsFile()
        {
            foreach (var pair in EffectsMap)
            {
                var effectData = GetEffectData(pair.Key);

                OptionsManager.EffectsFile.WriteValue(pair.Key, $"{(m_TreeMenuItemsMap?[pair.Key].IsChecked is true ? 1 : 0)}"
                    + $",{(int)effectData.TimedType.GetValueOrDefault(EffectTimedType.NotTimed)}"
                    + $",{effectData.CustomTime.GetValueOrDefault(0)}"
                    + $",{effectData.WeightMult.GetValueOrDefault(0)}"
                    + $",{(effectData.TimedType.GetValueOrDefault(EffectTimedType.NotTimed) == EffectTimedType.Permanent ? 1 : 0)}"
                    + $",{(effectData.ExcludedFromVoting.GetValueOrDefault(false) ? 1 : 0)}"
                    + $",\"{(string.IsNullOrEmpty(effectData.CustomName) ? "" : effectData.CustomName)}\""
                    + $",{effectData.ShortcutKeycode.GetValueOrDefault(0)}");
            }

            OptionsManager.EffectsFile.WriteFile();
        }

        private void InitEffectsTreeView()
        {
            effects_user_effects_search.Clear();

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
                menuItem.OnConfigureClick = () =>
                {
                    var effectInfo = EffectsMap[effectMisc.EffectId];

                    var effectConfig = new EffectConfig(effectMisc.EffectId, effectData, effectInfo);
                    effectConfig.ShowDialog();

                    if (!effectConfig.IsSaved)
                        return;

                    effectData = effectConfig.GetNewData();
                    if (m_EffectDataMap is not null)
                        m_EffectDataMap[effectMisc.EffectId] = effectData;
                    menuItem.IsColored = effectData.TimedType == EffectTimedType.Permanent;
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

            m_TreeMenuItemsFiltered = m_TreeMenuItemsAll.ToList();
            effects_user_effects_tree_view.ItemsSource = m_TreeMenuItemsFiltered;

            meta_effects_tree_view.Items.Clear();
            meta_effects_tree_view.Items.Add(metaParentItem);

            foreach (var treeMenuItem in m_TreeMenuItemsAll.Append(metaParentItem))
                treeMenuItem.UpdateCheckedAccordingToChildrenStatus();
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
            effects_user_effects_tree_view.Items.Refresh();
        }

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersPreviewTextInput(sender, e);
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
        {
            Utils.HandleNoSpacePreviewKeyDown(sender, e);
        }

        private void NoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Utils.HandleNoCopyPastePreviewExecuted(sender, e);
        }

        private void OnUserSaveClick(object sender, RoutedEventArgs e)
        {
            if (OptionsManager.ConfigFile.HasCompatFile("config.ini") || OptionsManager.TwitchFile.HasCompatFile("twitch.ini")
                || OptionsManager.EffectsFile.HasCompatFile("effects.ini"))
                if (MessageBox.Show("Note: Config files reside inside the configs/ subdirectory now. Clicking OK will move the files there. " +
                    "If you want to play older versions of the mod you will have to move them back. Continue?", "ChaosModV", MessageBoxButton.OKCancel, MessageBoxImage.Warning)
                    != MessageBoxResult.OK)
                    return;

            WriteConfigFile();
            WriteEffectsFile();

            foreach (var tab in m_Tabs)
                tab.Value.OnSaveValues();

            OptionsManager.WriteFiles();

            // Reload saved config to show the "new" (saved) settings
            foreach (var tab in m_Tabs)
                tab.Value.OnLoadValues();

            OptionsManager.DeleteCompatFiles();

            MessageBox.Show("Saved config!\nMake sure to press CTRL + L in-game twice if mod is already running to reload the config.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void OnUserResetClick(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Are you sure you want to reset your config?", "ChaosModV",
                MessageBoxButton.YesNo, MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                OptionsManager.ResetFiles();

                result = MessageBox.Show("Do you want to reset your voting settings too?", "ChaosModV",
                    MessageBoxButton.YesNo, MessageBoxImage.Question);

                if (result == MessageBoxResult.Yes)
                    OptionsManager.TwitchFile.ResetFile();

                Init();

                MessageBox.Show("Config has been reverted to default settings!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        public void OpenModPageEvent(object sender, RoutedEventArgs eventArgs)
        {
            System.Diagnostics.Process.Start("https://www.gta5-mods.com/scripts/chaos-mod-v-beta");
        }
    }
}
