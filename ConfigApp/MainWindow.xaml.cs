using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Shared;
using ZstdSharp;

using static ConfigApp.Effects;

using MessageBox = System.Windows.MessageBox;

namespace ConfigApp
{
    public partial class MainWindow : Window
    {
        private bool m_bInitializedTitle = false;
        
        private OptionsFile m_ConfigFile = new OptionsFile("config.ini");
        private OptionsFile m_TwitchFile = new OptionsFile("twitch.ini");
        private OptionsFile m_EffectsFile = new OptionsFile("effects.ini");

        private Dictionary<string, TreeMenuItem> m_TreeMenuItemsMap;
        private Dictionary<string, EffectData> m_EffectDataMap;

        private ObservableCollection<WorkshopSubmissionItem> m_WorkshopSubmissionItems = new ObservableCollection<WorkshopSubmissionItem>();

        public MainWindow()
        {
            Init();
        }

        private void Init()
        {
            InitializeComponent();

            twitch_user_overlay_mode.ItemsSource = new string[]
            {
                "Chat Messages",
                "In-Game Overlay",
                "OBS Overlay"
            };

            if (!m_bInitializedTitle)
            {
                m_bInitializedTitle = true;

                Title += " (v" + Info.VERSION + ")";
            }

            CheckForUpdates();

            ParseConfigFile();
            ParseTwitchFile();

            InitEffectsTreeView();

            ParseEffectsFile();

            InitTwitchTab();

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

        private async void CheckForUpdates()
        {
            HttpClient httpClient = new HttpClient();

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
            m_ConfigFile.ReadFile();

            misc_user_effects_spawn_dur.Text = m_ConfigFile.ReadValue("NewEffectSpawnTime", "30");
            misc_user_effects_timed_dur.Text = m_ConfigFile.ReadValue("EffectTimedDur", "90");
            misc_user_effects_random_seed.Text = m_ConfigFile.ReadValue("Seed");
            misc_user_effects_timed_short_dur.Text = m_ConfigFile.ReadValue("EffectTimedShortDur", "30");
            misc_user_effects_clear_enable.IsChecked = m_ConfigFile.ReadValueBool("EnableClearEffectsShortcut", true);
            misc_user_effects_drawtimer_disable.IsChecked = m_ConfigFile.ReadValueBool("DisableTimerBarDraw", false);
            misc_user_effects_drawtext_disable.IsChecked = m_ConfigFile.ReadValueBool("DisableEffectTextDraw", false);
            misc_user_toggle_mod_shortcut.IsChecked = m_ConfigFile.ReadValueBool("EnableToggleModShortcut", true);
            misc_user_effects_menu_enable.IsChecked = m_ConfigFile.ReadValueBool("EnableDebugMenu", false);
            misc_user_effects_timer_pause_shortcut_enable.IsChecked = m_ConfigFile.ReadValueBool("EnablePauseTimerShortcut", false);
            misc_user_effects_max_running_effects.Text = m_ConfigFile.ReadValue("MaxParallelRunningEffects", "99");
            if (m_ConfigFile.HasKey("EffectTimerColor"))
            {
                misc_user_effects_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_ConfigFile.ReadValue("EffectTimerColor"));
            }
            if (m_ConfigFile.HasKey("EffectTextColor"))
            {
                misc_user_effects_text_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_ConfigFile.ReadValue("EffectTextColor"));
            }
            if (m_ConfigFile.HasKey("EffectTimedTimerColor"))
            {
                misc_user_effects_effect_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_ConfigFile.ReadValue("EffectTimedTimerColor"));
            }
            misc_user_effects_disable_startup.IsChecked = m_ConfigFile.ReadValueBool("DisableStartup", false);
            misc_user_effects_enable_group_weighting.IsChecked = m_ConfigFile.ReadValueBool("EnableGroupWeightingAdjustments", true);
            misc_user_effects_enable_failsafe.IsChecked = m_ConfigFile.ReadValueBool("EnableFailsafe", true);
            misc_user_anti_softlock_shortcut.IsChecked = m_ConfigFile.ReadValueBool("EnableAntiSoftlockShortcut", true);

            // Meta Effects
            meta_effects_spawn_dur.Text = m_ConfigFile.ReadValue("NewMetaEffectSpawnTime", "600");
            meta_effects_timed_dur.Text = m_ConfigFile.ReadValue("MetaEffectDur", "95");
            meta_effects_short_timed_dur.Text = m_ConfigFile.ReadValue("MetaShortEffectDur", "65");
        }

        private void WriteConfigFile()
        {
            m_ConfigFile.WriteValue("NewEffectSpawnTime", misc_user_effects_spawn_dur.Text);
            m_ConfigFile.WriteValue("EffectTimedDur", misc_user_effects_timed_dur.Text);
            m_ConfigFile.WriteValue("Seed", misc_user_effects_random_seed.Text);
            m_ConfigFile.WriteValue("EffectTimedShortDur", misc_user_effects_timed_short_dur.Text);
            m_ConfigFile.WriteValue("EnableClearEffectsShortcut", misc_user_effects_clear_enable.IsChecked.Value);
            m_ConfigFile.WriteValue("DisableTimerBarDraw", misc_user_effects_drawtimer_disable.IsChecked.Value);
            m_ConfigFile.WriteValue("DisableEffectTextDraw", misc_user_effects_drawtext_disable.IsChecked.Value);
            m_ConfigFile.WriteValue("EnableToggleModShortcut", misc_user_toggle_mod_shortcut.IsChecked.Value);
            m_ConfigFile.WriteValue("EnableDebugMenu", misc_user_effects_menu_enable.IsChecked.Value);
            m_ConfigFile.WriteValue("EnablePauseTimerShortcut", misc_user_effects_timer_pause_shortcut_enable.IsChecked.Value);
            m_ConfigFile.WriteValue("EffectTimerColor", misc_user_effects_timer_color.SelectedColor.ToString());
            m_ConfigFile.WriteValue("EffectTextColor", misc_user_effects_text_color.SelectedColor.ToString());
            m_ConfigFile.WriteValue("EffectTimedTimerColor", misc_user_effects_effect_timer_color.SelectedColor.ToString());
            m_ConfigFile.WriteValue("DisableStartup", misc_user_effects_disable_startup.IsChecked.Value);
            m_ConfigFile.WriteValue("EnableGroupWeightingAdjustments", misc_user_effects_enable_group_weighting.IsChecked.Value);
            m_ConfigFile.WriteValue("EnableFailsafe", misc_user_effects_enable_failsafe.IsChecked.Value);
            int runningEffects;
            if (int.TryParse(misc_user_effects_max_running_effects.Text, out runningEffects) && runningEffects > 0)
            {
                m_ConfigFile.WriteValue("MaxParallelRunningEffects", misc_user_effects_max_running_effects.Text);
            }
            m_ConfigFile.WriteValue("EnableAntiSoftlockShortcut", misc_user_anti_softlock_shortcut.IsChecked.Value);

            // Meta Effects
            m_ConfigFile.WriteValue("NewMetaEffectSpawnTime", meta_effects_spawn_dur.Text);
            m_ConfigFile.WriteValue("MetaEffectDur", meta_effects_timed_dur.Text);
            m_ConfigFile.WriteValue("MetaShortEffectDur", meta_effects_short_timed_dur.Text);
            
            m_ConfigFile.WriteFile();
        }

        private void ParseTwitchFile()
        {
            m_TwitchFile.ReadFile();

            twitch_user_agreed.IsChecked = m_TwitchFile.ReadValueBool("EnableTwitchVoting", false);
            twitch_user_channel_name.Text = m_TwitchFile.ReadValue("TwitchChannelName");
            twitch_user_user_name.Text = m_TwitchFile.ReadValue("TwitchUserName");
            twitch_user_channel_oauth.Password = m_TwitchFile.ReadValue("TwitchChannelOAuth");
            twitch_user_effects_secs_before_chat_voting.Text = m_TwitchFile.ReadValue("TwitchVotingSecsBeforeVoting", "0");
            twitch_user_overlay_mode.SelectedIndex = m_TwitchFile.ReadValueInt("TwitchVotingOverlayMode", 0);
            twitch_user_chance_system_enable.IsChecked = m_TwitchFile.ReadValueBool("TwitchVotingChanceSystem", false);
            twitch_user_chance_system_retain_chance_enable.IsChecked = m_TwitchFile.ReadValueBool("TwitchVotingChanceSystemRetainChance", true);
            twitch_user_random_voteable_enable.IsChecked = m_TwitchFile.ReadValueBool("TwitchRandomEffectVoteableEnable", true);
            twitch_permitted_usernames.Text = m_TwitchFile.ReadValue("TwitchPermittedUsernames");
        }

        private void WriteTwitchFile()
        {
            m_TwitchFile.WriteValue("EnableTwitchVoting", twitch_user_agreed.IsChecked.Value);
            m_TwitchFile.WriteValue("TwitchChannelName", twitch_user_channel_name.Text);
            m_TwitchFile.WriteValue("TwitchUserName", twitch_user_user_name.Text);
            m_TwitchFile.WriteValue("TwitchChannelOAuth", twitch_user_channel_oauth.Password);
            m_TwitchFile.WriteValue("TwitchVotingSecsBeforeVoting", twitch_user_effects_secs_before_chat_voting.Text);
            m_TwitchFile.WriteValue("TwitchVotingOverlayMode", twitch_user_overlay_mode.SelectedIndex);
            m_TwitchFile.WriteValue("TwitchVotingChanceSystem", twitch_user_chance_system_enable.IsChecked.Value);
            m_TwitchFile.WriteValue("TwitchVotingChanceSystemRetainChance", twitch_user_chance_system_retain_chance_enable.IsChecked.Value);
            m_TwitchFile.WriteValue("TwitchRandomEffectVoteableEnable", twitch_user_random_voteable_enable.IsChecked.Value);
            m_TwitchFile.WriteValue("TwitchPermittedUsernames", twitch_permitted_usernames.Text);

            m_TwitchFile.WriteFile();
        }

        private void ParseEffectsFile()
        {
            m_EffectsFile.ReadFile();

            foreach (string key in m_EffectsFile.GetKeys())
            {
                string value = m_EffectsFile.ReadValue(key);

                // Split by comma, ignoring commas in between quotation marks
                string[] values = Regex.Split(value, ",(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))");

                if (!EffectsMap.TryGetValue(key, out EffectInfo effectInfo))
                {
                    continue;
                }

                EffectTimedType effectTimedType = effectInfo.IsShort ? EffectTimedType.TimedShort : EffectTimedType.TimedNormal;
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

                m_EffectsFile.WriteValue(pair.Key, $"{(m_TreeMenuItemsMap[pair.Key].IsChecked ? 1 : 0)}"
                    + $",{(effectData.TimedType == EffectTimedType.TimedNormal ? 0 : 1)}"
                    + $",{effectData.CustomTime},{effectData.WeightMult},{(effectData.Permanent ? 1 : 0)},{(effectData.ExcludedFromVoting ? 1 : 0)}"
                    + $",\"{(string.IsNullOrEmpty(effectData.CustomName) ? "" : effectData.CustomName)}\""
                    + $",{(effectData.Shortcut)}");
            }

            m_EffectsFile.WriteFile();
        }

        private void InitEffectsTreeView()
        {
            m_TreeMenuItemsMap = new Dictionary<string, TreeMenuItem>();
            m_EffectDataMap = new Dictionary<string, EffectData>();

            TreeMenuItem playerParentItem = new TreeMenuItem("Player");
            TreeMenuItem vehicleParentItem = new TreeMenuItem("Vehicle");
            TreeMenuItem pedsParentItem = new TreeMenuItem("Peds");
            TreeMenuItem screenParentItem = new TreeMenuItem("Screen");
            TreeMenuItem timeParentItem = new TreeMenuItem("Time");
            TreeMenuItem weatherParentItem = new TreeMenuItem("Weather");
            TreeMenuItem miscParentItem = new TreeMenuItem("Misc");
            TreeMenuItem metaParentItem = new TreeMenuItem("Meta");

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
        }

        void InitTwitchTab()
        {
            TwitchTabHandleAgreed();
        }

        void TwitchTabHandleAgreed()
        {
            bool agreed = twitch_user_agreed.IsChecked.GetValueOrDefault();

            twitch_user_channel_name_label.IsEnabled = agreed;
            twitch_user_channel_name.IsEnabled = agreed;
            twitch_user_channel_oauth_label.IsEnabled = agreed;
            twitch_user_channel_oauth.IsEnabled = agreed;
            twitch_user_user_name_label.IsEnabled = agreed;
            twitch_user_user_name.IsEnabled = agreed;
            twitch_user_effects_secs_before_chat_voting_label.IsEnabled = agreed;
            twitch_user_effects_secs_before_chat_voting.IsEnabled = agreed;
            twitch_user_overlay_mode_label.IsEnabled = agreed;
            twitch_user_overlay_mode.IsEnabled = agreed;
            twitch_user_chance_system_enable_label.IsEnabled = agreed;
            twitch_user_chance_system_enable.IsEnabled = agreed;
            twitch_user_chance_system_retain_chance_enable_label.IsEnabled = agreed;
            twitch_user_chance_system_retain_chance_enable.IsEnabled = agreed;
            twitch_user_random_voteable_enable.IsEnabled = agreed;
            twitch_user_random_voteable_enable_label.IsEnabled = agreed;
            twitch_permitted_usernames.IsEnabled = agreed;
            twitch_permitted_usernames_label.IsEnabled = agreed;
        }

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersPreviewTextInput(e);
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
        {
            Utils.HandleNoSpacePreviewKeyDown(e);
        }

        private void NoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Utils.HandleNoCopyPastePreviewExecuted(e);
        }

        private void user_save_Click(object sender, RoutedEventArgs e)
        {
            WriteConfigFile();
            WriteTwitchFile();
            WriteEffectsFile();

            // Reload saved config to show the "new" (saved) settings
            ParseConfigFile();
            ParseTwitchFile();

            MessageBox.Show("Saved config!\nMake sure to press CTRL + L in-game twice if mod is already running to reload the config.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void user_reset_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Are you sure you want to reset your config?", "ChaosModV",
                MessageBoxButton.YesNo, MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                m_ConfigFile.ResetFile();

                m_EffectsFile.ResetFile();

                result = MessageBox.Show("Do you want to reset your Twitch settings too?", "ChaosModV",
                    MessageBoxButton.YesNo, MessageBoxImage.Question);

                if (result == MessageBoxResult.Yes)
                {
                    m_TwitchFile.ResetFile();
                    ParseTwitchFile();

                    // Ensure all options are disabled in twitch tab again
                    TwitchTabHandleAgreed();
                }

                Init();

                MessageBox.Show("Config has been reverted to default settings!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private void twitch_user_agreed_Click(object sender, RoutedEventArgs e)
        {
            TwitchTabHandleAgreed();
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
                    Key shortcut = (Key)effectConfig.effectconf_effect_shortcut_combo.SelectedItem;
                    effectData.Shortcut = KeyInterop.VirtualKeyFromKey(shortcut);
                }
            }
        }

        private void DisplayWorkshopFetchContentFailure()
        {
            MessageBox.Show("Error occured while trying to fetch submissions from server! Please try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        private void HandleWorkshopSubmissionsSearchFilter()
        {
            var transformedText = workshop_search.Text.Trim().ToLower();
            var view = CollectionViewSource.GetDefaultView(m_WorkshopSubmissionItems);
            view.Filter = submissionItem => ((WorkshopSubmissionItem)submissionItem).Name.ToLower().Contains(transformedText);
        }

        private void ParseWorkshopSubmissionsFile(byte[] compressedFileContent)
        {
            {
                var decompressor = new Decompressor();
                var decompressed = decompressor.Unwrap(compressedFileContent);
                string fileContent = Encoding.UTF8.GetString(decompressed.ToArray());

                var json = JObject.Parse(fileContent);

                // Only clear after trying to parse
                m_WorkshopSubmissionItems.Clear();

                foreach (var submissionObject in json["submissions"].ToObject<Dictionary<string, dynamic>>())
                {
                    var submissionId = submissionObject.Key;
                    var submissionData = submissionObject.Value;

                    // Submission has no data
                    if (submissionData.lastupdated == 0)
                    {
                        continue;
                    }

                    WorkshopSubmissionItem submission = new WorkshopSubmissionItem();
                    submission.Id = submissionId;
                    submission.Name = submissionData.name;
                    submission.Author = submissionData.author;
                    submission.Description = submissionData.description;
                    if (submission.Description.Length == 0)
                    {
                        submission.Description = "No Description";
                    }
                    submission.Version = $"v{submissionData.version}";
                    submission.LastUpdated = submissionData.lastupdated;
                    submission.Sha256 = submissionData.sha256;

                    m_WorkshopSubmissionItems.Add(submission);
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

                    var version = (string)json["version"];
                    var lastUpdated = (int)json["lastupdated"];
                    var sha256 = (string)json["sha256"];

                    var foundSubmissionItem = m_WorkshopSubmissionItems.FirstOrDefault((submissionItem) => { return submissionItem.Id == id; });
                    if (foundSubmissionItem == null)
                    {
                        var result = MessageBox.Show($"Local submission \"{id}\" does not exist remotely. Remove submission?", "ChaosModV", MessageBoxButton.YesNo, MessageBoxImage.Warning);
                        if (result == MessageBoxResult.Yes)
                        {
                            try
                            {
                                Directory.Delete(directory, true);
                            }
                            catch (Exception)
                            {
                                MessageBox.Show($"Couldn't access \"{directory}\". Try deleting it manually!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                            }
                        }
                        continue;
                    }
                    else if (foundSubmissionItem.Version != version || foundSubmissionItem.LastUpdated != lastUpdated || foundSubmissionItem.Sha256 != sha256)
                    {
                        foundSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.UpdateAvailable;
                    }
                    else
                    {
                        foundSubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                    }
                }
                catch (Exception exception) when (exception is JsonException || exception is ZstdException)
                {
                    MessageBox.Show($"Local submission \"{id}\" has a corrupt metadata.json.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Warning);
                    continue;
                }
            }

            m_WorkshopSubmissionItems = new ObservableCollection<WorkshopSubmissionItem>(m_WorkshopSubmissionItems.OrderBy(item => item.InstallState).ThenBy(item => item.Name));

            workshop_submission_items_datagrid.ItemsSource = m_WorkshopSubmissionItems;

            HandleWorkshopSubmissionsSearchFilter();
        }

        private async Task ForceRefreshWorkshopContentFromRemote()
        {
            HttpClient httpClient = new HttpClient();
            try
            {
                var result = await httpClient.GetAsync("https://chaos.gopong.dev/workshop/fetch_submissions");
                if (result.IsSuccessStatusCode)
                {
                    var compressedResult = await result.Content?.ReadAsByteArrayAsync();

                    ParseWorkshopSubmissionsFile(compressedResult);

                    // Cache submissions
                    File.WriteAllBytes("workshop/submissions_cached.json.zst", compressedResult);
                }
            }
            catch (HttpRequestException)
            {
                DisplayWorkshopFetchContentFailure();
            }
            catch (Exception exception) when (exception is JsonException || exception is ZstdException)
            {
                MessageBox.Show($"Remote provided a malformed master submissions file! Can not fetch available submissions.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void workshop_tab_Click(object sender, MouseButtonEventArgs e)
        {
            // Only fetch them once
            if (m_WorkshopSubmissionItems.Count > 0)
            {
                return;
            };

            byte[] fileContent = null;
            // Use cached content if existing (and accessible), otherwise fall back to server request
            if (File.Exists("workshop/submissions_cached.json.zst"))
            {
                try
                {
                    fileContent = File.ReadAllBytes("workshop/submissions_cached.json.zst");
                }
                catch (IOException)
                {

                }
            }

            if (fileContent != null)
            {
                try
                {
                    ParseWorkshopSubmissionsFile(fileContent);
                }
                catch (JsonException)
                {
                    // Cached file is corrupt, force reload
                    await ForceRefreshWorkshopContentFromRemote();
                }
            }
            else
            {
                await ForceRefreshWorkshopContentFromRemote();
            }
        }

        private async void workshop_refresh_Click(object sender, RoutedEventArgs e)
        {
            var button = (Button)sender;

            button.IsEnabled = false;
            await ForceRefreshWorkshopContentFromRemote();
            button.IsEnabled = true;
        }

        private void workshop_search_TextChanged(object sender, TextChangedEventArgs e)
        {
            HandleWorkshopSubmissionsSearchFilter();
        }

        private void contribute_modpage_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://www.gta5-mods.com/scripts/chaos-mod-v-beta");
        }

        private void contribute_github_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://github.com/gta-chaos-mod/ChaosModV");
        }

        private void contribute_donate_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://paypal.me/EmrCue");
        }

        private void contribute_discord_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://discord.gg/w2tDeKVaF9");
        }
    }
}