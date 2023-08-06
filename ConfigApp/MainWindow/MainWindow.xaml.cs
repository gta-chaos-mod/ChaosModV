using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

using static ConfigApp.Effects;
using Color = System.Windows.Media.Color;
using MessageBox = System.Windows.MessageBox;

namespace ConfigApp.MainWindow
{
    public partial class MainWindow : Window
    {
        private Dictionary<string, Tab> m_Tabs = new Dictionary<string, Tab>
        {
            //{ "Meta", new MetaTab() },
            { "Misc", new MiscTab() },
            { "Twitch", new TwitchTab() },
            { "Workshop", new WorkshopTab() },
            { "More", new MoreTab() }
        };
        private Dictionary<string, TabItem> m_TabItems = new Dictionary<string, TabItem>();

        private bool m_bInitializedTitle = false;

        private Dictionary<string, TreeMenuItem> m_TreeMenuItemsMap;
        private Dictionary<string, EffectData> m_EffectDataMap;

        public MainWindow()
        {
            Init();
        }

        private void Init()
        {
            InitializeComponent();

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

            if (!m_bInitializedTitle)
            {
                m_bInitializedTitle = true;

                Title += " (v" + Info.VERSION + ")";
            }

            CheckForUpdates();

            OptionsManager.ReadFiles();

            foreach (var tab in m_Tabs)
            {
                tab.Value.OnLoadValues();
            }

            ParseConfigFile();

            InitEffectsTreeView();

            ParseEffectsFile();

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
            {
                return;
            }

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
                {
                    update_available_button.Visibility = Visibility.Visible;
                }
                else
                {
                    update_available_label.Text = "You are on the newest version of the mod!";
                }
            }
            catch (HttpRequestException)
            {
                update_available_label.Text = "Unable to check for new updates!";
            }
        }

        private EffectData GetEffectData(string effectId)
        {
            // Create EffectData in case effect wasn't saved yet
            if (!m_EffectDataMap.TryGetValue(effectId, out EffectData effectData))
            {
                effectData = new EffectData(EffectsMap[effectId].IsShort ? EffectTimedType.TimedShort : EffectTimedType.TimedNormal, -1, 5, false, false, null, 0);

                m_EffectDataMap.Add(effectId, effectData);
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
                string value = OptionsManager.EffectsFile.ReadValue(key);

                // Split by comma, ignoring commas in between quotation marks
                string[] values = Regex.Split(value, ",(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))");

                if (!EffectsMap.TryGetValue(key, out EffectInfo effectInfo))
                {
                    continue;
                }

                var effectTimedType = effectInfo.IsShort ? EffectTimedType.TimedShort : EffectTimedType.TimedNormal;
                int effectTimedTime = -1;
                int effectWeight = 5;
                bool effectPermanent = false;
                bool effectExcludedFromVoting = false;
                string effectCustomName = null;
                int effectShortcut = 0;

                // Compatibility checks, previous versions had less options
                if (values.Length >= 4)
                {
                    Enum.TryParse(values[1], out effectTimedType);
                    int.TryParse(values[2], out effectTimedTime);
                    int.TryParse(values[3], out effectWeight);

                    if (values.Length >= 5)
                    {
                        int tmp;

                        int.TryParse(values[4], out tmp);
                        effectPermanent = tmp != 0;

                        if (values.Length >= 6)
                        {
                            int.TryParse(values[5], out tmp);
                            effectExcludedFromVoting = tmp != 0;

                            if (values.Length >= 7)
                            {
                                effectCustomName = values[6] == "0" ? null : values[6].Trim('\"');
                                if (values.Length >= 8)
                                {
                                    int.TryParse(values[7], out effectShortcut);
                                }
                            }
                        }
                    }
                }

                int.TryParse(values[0], out int enabled);
                m_TreeMenuItemsMap[key].IsChecked = enabled == 0 ? false : true;

                m_EffectDataMap.Add(key, new EffectData(effectTimedType, effectTimedTime, effectWeight, effectPermanent,
                    effectExcludedFromVoting, effectCustomName, effectShortcut));
            }
        }

        private void WriteEffectsFile()
        {
            foreach (var pair in EffectsMap)
            {
                EffectData effectData = GetEffectData(pair.Key);

                OptionsManager.EffectsFile.WriteValue(pair.Key, $"{(m_TreeMenuItemsMap[pair.Key].IsChecked ? 1 : 0)}"
                    + $",{(effectData.TimedType == EffectTimedType.TimedNormal ? 0 : 1)}"
                    + $",{effectData.CustomTime},{effectData.WeightMult},{(effectData.Permanent ? 1 : 0)},{(effectData.ExcludedFromVoting ? 1 : 0)}"
                    + $",\"{(string.IsNullOrEmpty(effectData.CustomName) ? "" : effectData.CustomName)}\""
                    + $",{(effectData.Shortcut)}");
            }

            OptionsManager.EffectsFile.WriteFile();
        }

        private void InitEffectsTreeView()
        {
            m_TreeMenuItemsMap = new Dictionary<string, TreeMenuItem>();
            m_EffectDataMap = new Dictionary<string, EffectData>();

            var playerParentItem = new TreeMenuItem("Player");
            var vehicleParentItem = new TreeMenuItem("Vehicle");
            var pedsParentItem = new TreeMenuItem("Peds");
            var screenParentItem = new TreeMenuItem("Screen");
            var timeParentItem = new TreeMenuItem("Time");
            var weatherParentItem = new TreeMenuItem("Weather");
            var miscParentItem = new TreeMenuItem("Misc");
            var metaParentItem = new TreeMenuItem("Meta");

            var sortedEffects = new SortedDictionary<string, Tuple<string, EffectCategory>>();

            foreach (var pair in EffectsMap)
            {
                sortedEffects.Add(pair.Value.Name, new Tuple<string, EffectCategory>(pair.Key, pair.Value.EffectCategory));
            }
            
            foreach (var effect in sortedEffects)
            {
                var effectTuple = effect.Value;

                TreeMenuItem menuItem = new TreeMenuItem(effect.Key);
                m_TreeMenuItemsMap.Add(effectTuple.Item1, menuItem);

                switch (effectTuple.Item2)
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

            effects_user_effects_tree_view.Items.Clear();
            effects_user_effects_tree_view.Items.Add(playerParentItem);
            effects_user_effects_tree_view.Items.Add(vehicleParentItem);
            effects_user_effects_tree_view.Items.Add(pedsParentItem);
            effects_user_effects_tree_view.Items.Add(screenParentItem);
            effects_user_effects_tree_view.Items.Add(timeParentItem);
            effects_user_effects_tree_view.Items.Add(weatherParentItem);
            effects_user_effects_tree_view.Items.Add(miscParentItem);

            meta_effects_tree_view.Items.Clear();
            meta_effects_tree_view.Items.Add(metaParentItem);

            // We want every effect to be enabled by default, also this is necessary to make the enabled effect counter show up
            foreach (var treeMenuItem in m_TreeMenuItemsMap)
            {
                treeMenuItem.Value.IsChecked = true;
            }
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

        private void user_save_Click(object sender, RoutedEventArgs e)
        {
            if (OptionsManager.ConfigFile.HasCompatFile() || OptionsManager.TwitchFile.HasCompatFile() || OptionsManager.EffectsFile.HasCompatFile())
            {
                if (MessageBox.Show("Note: Config files reside inside the configs/ subdirectory now. Clicking OK will move the files there. " +
                    "If you want to play older versions of the mod you will have to move them back. Continue?", "ChaosModV", MessageBoxButton.OKCancel, MessageBoxImage.Warning)
                    != MessageBoxResult.OK)
                {
                    return;
                }
            }

            WriteConfigFile();
            WriteEffectsFile();

            foreach (var tab in m_Tabs)
            {
                tab.Value.OnSaveValues();
            }

            OptionsManager.WriteFiles();

            // Reload saved config to show the "new" (saved) settings
            foreach (var tab in m_Tabs)
            {
                tab.Value.OnLoadValues();
            }

            OptionsManager.DeleteCompatFiles();

            MessageBox.Show("Saved config!\nMake sure to press CTRL + L in-game twice if mod is already running to reload the config.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void user_reset_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Are you sure you want to reset your config?", "ChaosModV",
                MessageBoxButton.YesNo, MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                OptionsManager.ResetFiles();

                result = MessageBox.Show("Do you want to reset your Twitch settings too?", "ChaosModV",
                    MessageBoxButton.YesNo, MessageBoxImage.Question);

                if (result == MessageBoxResult.Yes)
                {
                    OptionsManager.TwitchFile.ResetFile();
                }

                Init();

                MessageBox.Show("Config has been reverted to default settings!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private void effect_user_config_Click(object sender, RoutedEventArgs e)
        {
            TreeMenuItem curTreeMenuItem = (TreeMenuItem)((TreeViewItem)((Grid)((Border)((ContentPresenter)((StackPanel)((Button)sender).Parent).TemplatedParent).Parent).Parent).TemplatedParent).DataContext;

            string effectId = null;
            foreach (var pair in m_TreeMenuItemsMap)
            {
                if (pair.Value == curTreeMenuItem)
                {
                    effectId = pair.Key;

                    break;
                }
            }

            if (effectId != null)
            {
                var effectInfo = EffectsMap[effectId];
                var effectData = GetEffectData(effectId);

                var effectConfig = new EffectConfig(effectId, effectData, effectInfo);
                effectConfig.ShowDialog();

                if (effectConfig.IsSaved)
                {
                    effectData.TimedType = effectConfig.effectconf_timer_type_enable.IsChecked.Value ? (EffectTimedType)effectConfig.effectconf_timer_type.SelectedIndex
                        : effectInfo.IsShort ? EffectTimedType.TimedShort : EffectTimedType.TimedNormal;
                    effectData.CustomTime = effectConfig.effectconf_timer_time_enable.IsChecked.Value
                        ? effectConfig.effectconf_timer_time.Text.Length > 0 ? int.Parse(effectConfig.effectconf_timer_time.Text) : -1 : -1;
                    effectData.Permanent = effectConfig.effectconf_timer_permanent_enable.IsChecked.Value;
                    effectData.WeightMult = effectConfig.effectconf_effect_weight_mult.SelectedIndex + 1;
                    effectData.ExcludedFromVoting = effectConfig.effectconf_exclude_voting_enable.IsChecked.Value;
                    effectData.CustomName = effectConfig.effectconf_effect_custom_name.Text.Trim();
                    effectData.Shortcut = effectConfig.EffectShortcut;
                }
            }
        }

        public void OpenModPageEvent(object sender, RoutedEventArgs eventArgs)
        {
            System.Diagnostics.Process.Start("https://www.gta5-mods.com/scripts/chaos-mod-v-beta");
        }
    }
}