using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using Shared; 

using static ConfigApp.Effects;

namespace ConfigApp
{
    public partial class MainWindow : Window
    {
        private bool m_initializedTitle = false;

        private OptionsFile m_configFile = new OptionsFile("config.ini");
        private OptionsFile m_twitchFile = new OptionsFile("twitch.ini");
        private OptionsFile m_effectsFile = new OptionsFile("effects.ini");

        private Dictionary<EffectType, TreeMenuItem> m_treeMenuItemsMap;
        private Dictionary<EffectType, EffectData> m_effectDataMap;

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

            if (!m_initializedTitle)
            {
                m_initializedTitle = true;

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
                    update_available_label.Content = "You are on the newest version of the mod!";
                }
            }
            catch (HttpRequestException)
            {
                update_available_label.Content = "Unable to check for new updates!";
            }
        }

        private EffectData GetEffectData(EffectType effectType)
        {
            // Create EffectData in case effect wasn't saved yet
            if (!m_effectDataMap.TryGetValue(effectType, out EffectData effectData))
            {
                effectData = new EffectData(EffectsMap[effectType].IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL, -1, 5, false, false, null);

                m_effectDataMap.Add(effectType, effectData);
            }

            return effectData;
        }

        private void ParseConfigFile()
        {
            m_configFile.ReadFile();

            misc_user_effects_spawn_dur.Text = m_configFile.ReadValue("NewEffectSpawnTime", "30");
            misc_user_effects_timed_dur.Text = m_configFile.ReadValue("EffectTimedDur", "90");
            misc_user_effects_random_seed.Text = m_configFile.ReadValue("Seed");
            misc_user_effects_timed_short_dur.Text = m_configFile.ReadValue("EffectTimedShortDur", "30");
            misc_user_effects_clear_enable.IsChecked = m_configFile.ReadValueBool("EnableClearEffectsShortcut", true);
            misc_user_effects_drawtimer_disable.IsChecked = m_configFile.ReadValueBool("DisableTimerBarDraw", false);
            misc_user_effects_drawtext_disable.IsChecked = m_configFile.ReadValueBool("DisableEffectTextDraw", false);
            misc_user_toggle_mod_shortcut.IsChecked = m_configFile.ReadValueBool("EnableToggleModShortcut", true);
            misc_user_effects_menu_enable.IsChecked = m_configFile.ReadValueBool("EnableDebugMenu", false);
            misc_user_effects_timer_pause_shortcut_enable.IsChecked = m_configFile.ReadValueBool("EnablePauseTimerShortcut", false);
            misc_user_toggle_mod_shortcut.IsChecked = m_configFile.ReadValueBool("EnableToggleModShortcut", true);
            if (m_configFile.HasKey("EffectTimerColor"))
            {
                misc_user_effects_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_configFile.ReadValue("EffectTimerColor"));
            }
            if (m_configFile.HasKey("EffectTextColor"))
            {
                misc_user_effects_text_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_configFile.ReadValue("EffectTextColor"));
            }
            if (m_configFile.HasKey("EffectTimedTimerColor"))
            {
                misc_user_effects_effect_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_configFile.ReadValue("EffectTimedTimerColor"));
            }
            misc_user_effects_disable_startup.IsChecked = m_configFile.ReadValueBool("DisableStartup", false);
            misc_user_effects_enable_group_weighting.IsChecked = m_configFile.ReadValueBool("EnableGroupWeightingAdjustments", true);
            misc_user_effects_enable_failsafe.IsChecked = m_configFile.ReadValueBool("EnableFailsafe", true);

            // Meta Effects
            meta_effects_spawn_dur.Text = m_configFile.ReadValue("NewMetaEffectSpawnTime", "600");
            meta_effects_timed_dur.Text = m_configFile.ReadValue("MetaEffectDur", "95");
            meta_effects_short_timed_dur.Text = m_configFile.ReadValue("MetaShortEffectDur", "65");
        }

        private void WriteConfigFile()
        {
            m_configFile.WriteValue("NewEffectSpawnTime", misc_user_effects_spawn_dur.Text);
            m_configFile.WriteValue("EffectTimedDur", misc_user_effects_timed_dur.Text);
            m_configFile.WriteValue("Seed", misc_user_effects_random_seed.Text);
            m_configFile.WriteValue("EffectTimedShortDur", misc_user_effects_timed_short_dur.Text);
            m_configFile.WriteValue("EnableClearEffectsShortcut", misc_user_effects_clear_enable.IsChecked.Value);
            m_configFile.WriteValue("DisableTimerBarDraw", misc_user_effects_drawtimer_disable.IsChecked.Value);
            m_configFile.WriteValue("DisableEffectTextDraw", misc_user_effects_drawtext_disable.IsChecked.Value);
            m_configFile.WriteValue("EnableToggleModShortcut", misc_user_toggle_mod_shortcut.IsChecked.Value);
            m_configFile.WriteValue("EnableDebugMenu", misc_user_effects_menu_enable.IsChecked.Value);
            m_configFile.WriteValue("EnablePauseTimerShortcut", misc_user_effects_timer_pause_shortcut_enable.IsChecked.Value);
            m_configFile.WriteValue("EffectTimerColor", misc_user_effects_timer_color.SelectedColor.ToString());
            m_configFile.WriteValue("EffectTextColor", misc_user_effects_text_color.SelectedColor.ToString());
            m_configFile.WriteValue("EffectTimedTimerColor", misc_user_effects_effect_timer_color.SelectedColor.ToString());
            m_configFile.WriteValue("DisableStartup", misc_user_effects_disable_startup.IsChecked.Value);
            m_configFile.WriteValue("EnableGroupWeightingAdjustments", misc_user_effects_enable_group_weighting.IsChecked.Value);
            m_configFile.WriteValue("EnableFailsafe", misc_user_effects_enable_failsafe.IsChecked.Value);

            // Meta Effects
            m_configFile.WriteValue("NewMetaEffectSpawnTime", meta_effects_spawn_dur.Text);
            m_configFile.WriteValue("MetaEffectDur", meta_effects_timed_dur.Text);
            m_configFile.WriteValue("MetaShortEffectDur", meta_effects_short_timed_dur.Text);
            
            m_configFile.WriteFile();
        }

        private void ParseTwitchFile()
        {
            m_twitchFile.ReadFile();

            twitch_user_agreed.IsChecked = m_twitchFile.ReadValueBool("EnableTwitchVoting", false);
            twitch_user_channel_name.Text = m_twitchFile.ReadValue("TwitchChannelName");
            twitch_user_user_name.Text = m_twitchFile.ReadValue("TwitchUserName");
            twitch_user_channel_oauth.Password = m_twitchFile.ReadValue("TwitchChannelOAuth");
            twitch_user_effects_secs_before_chat_voting.Text = m_twitchFile.ReadValue("TwitchVotingSecsBeforeVoting", "0");
            twitch_user_overlay_mode.SelectedIndex = m_twitchFile.ReadValueInt("TwitchVotingOverlayMode", 0);
            twitch_user_chance_system_enable.IsChecked = m_twitchFile.ReadValueBool("TwitchVotingChanceSystem", false);
            twitch_user_chance_system_retain_chance_enable.IsChecked = m_twitchFile.ReadValueBool("TwitchVotingChanceSystemRetainChance", true);
            twitch_user_random_voteable_enable.IsChecked = m_twitchFile.ReadValueBool("TwitchRandomEffectVoteableEnable", true);
        }

        private void WriteTwitchFile()
        {
            m_twitchFile.WriteValue("EnableTwitchVoting", twitch_user_agreed.IsChecked.Value);
            m_twitchFile.WriteValue("TwitchChannelName", twitch_user_channel_name.Text);
            m_twitchFile.WriteValue("TwitchUserName", twitch_user_user_name.Text);
            m_twitchFile.WriteValue("TwitchChannelOAuth", twitch_user_channel_oauth.Password);
            m_twitchFile.WriteValue("TwitchVotingSecsBeforeVoting", twitch_user_effects_secs_before_chat_voting.Text);
            m_twitchFile.WriteValue("TwitchVotingOverlayMode", twitch_user_overlay_mode.SelectedIndex);
            m_twitchFile.WriteValue("TwitchVotingChanceSystem", twitch_user_chance_system_enable.IsChecked.Value);
            m_twitchFile.WriteValue("TwitchVotingChanceSystemRetainChance", twitch_user_chance_system_retain_chance_enable.IsChecked.Value);
            m_twitchFile.WriteValue("TwitchRandomEffectVoteableEnable", twitch_user_random_voteable_enable.IsChecked.Value);

            m_twitchFile.WriteFile();
        }

        private void ParseEffectsFile()
        {
            m_effectsFile.ReadFile();

            foreach (string key in m_effectsFile.GetKeys())
            {
                string value = m_effectsFile.ReadValue(key);

                string[] values = value.Split(',');

                // Find EffectType from ID
                EffectType effectType = EffectType._EFFECT_ENUM_MAX;
                foreach (KeyValuePair<EffectType, EffectInfo> pair in EffectsMap)
                {
                    if (pair.Value.Id == key)
                    {
                        effectType = pair.Key;
                        break;
                    }
                }

                if (!EffectsMap.TryGetValue(effectType, out EffectInfo effectInfo))
                {
                    continue;
                }

                EffectTimedType effectTimedType = effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL;
                int effectTimedTime = -1;
                int effectWeight = 5;
                bool effectPermanent = false;
                bool effectExcludedFromVoting = false;
                string effectCustomName = null;

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
                                effectCustomName = values[6] == "0" ? null : values[6];
                            }
                        }
                    }
                }

                if (effectType != EffectType._EFFECT_ENUM_MAX)
                {
                    int.TryParse(values[0], out int enabled);
                    m_treeMenuItemsMap[effectType].IsChecked = enabled == 0 ? false : true;

                    m_effectDataMap.Add(effectType, new EffectData(effectTimedType, effectTimedTime, effectWeight, effectPermanent, effectExcludedFromVoting, effectCustomName));
                }
            }
        }

        private void WriteEffectsFile()
        {
            for (EffectType effectType = 0; effectType < EffectType._EFFECT_ENUM_MAX; effectType++)
            {
                EffectData effectData = GetEffectData(effectType);

                m_effectsFile.WriteValue(EffectsMap[effectType].Id, $"{(m_treeMenuItemsMap[effectType].IsChecked ? 1 : 0)},{(effectData.TimedType == EffectTimedType.TIMED_NORMAL ? 0 : 1)}"
                    + $",{effectData.CustomTime},{effectData.WeightMult},{(effectData.Permanent ? 1 : 0)},{(effectData.ExcludedFromVoting ? 1 : 0)}"
                    + $",{(string.IsNullOrEmpty(effectData.CustomName) ? "0" : effectData.CustomName)}");
            }

            m_effectsFile.WriteFile();
        }

        private void InitEffectsTreeView()
        {
            m_treeMenuItemsMap = new Dictionary<EffectType, TreeMenuItem>();
            m_effectDataMap = new Dictionary<EffectType, EffectData>();

            TreeMenuItem playerParentItem = new TreeMenuItem("Player");
            TreeMenuItem vehicleParentItem = new TreeMenuItem("Vehicle");
            TreeMenuItem pedsParentItem = new TreeMenuItem("Peds");
            TreeMenuItem timeParentItem = new TreeMenuItem("Time");
            TreeMenuItem weatherParentItem = new TreeMenuItem("Weather");
            TreeMenuItem miscParentItem = new TreeMenuItem("Misc");
            TreeMenuItem metaParentItem = new TreeMenuItem("Meta");

            SortedDictionary<string, Tuple<EffectType, EffectCategory>> sortedEffects = new SortedDictionary<string, Tuple<EffectType, EffectCategory>>();

            for (EffectType effectType = 0; effectType < EffectType._EFFECT_ENUM_MAX; effectType++)
            {
                EffectInfo effectInfo = EffectsMap[effectType];

                sortedEffects.Add(effectInfo.Name, new Tuple<EffectType, EffectCategory>(effectType, effectInfo.EffectCategory));
            }
            
            foreach (var effect in sortedEffects)
            {
                Tuple<EffectType, EffectCategory> effectTuple = effect.Value;

                TreeMenuItem menuItem = new TreeMenuItem(effect.Key);
                m_treeMenuItemsMap.Add(effectTuple.Item1, menuItem);

                switch (effectTuple.Item2)
                {
                    case EffectCategory.PLAYER:
                        playerParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.VEHICLE:
                        vehicleParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.PEDS:
                        pedsParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.TIME:
                        timeParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.WEATHER:
                        weatherParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.MISC:
                        miscParentItem.AddChild(menuItem);
                        break;
                    case EffectCategory.META:
                        metaParentItem.AddChild(menuItem);
                        break;
                }
            }

            effects_user_effects_tree_view.Items.Clear();
            effects_user_effects_tree_view.Items.Add(playerParentItem);
            effects_user_effects_tree_view.Items.Add(vehicleParentItem);
            effects_user_effects_tree_view.Items.Add(pedsParentItem);
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
                m_configFile.ResetFile();

                m_effectsFile.ResetFile();

                result = MessageBox.Show("Do you want to reset your Twitch settings too?", "ChaosModV",
                    MessageBoxButton.YesNo, MessageBoxImage.Question);

                if (result == MessageBoxResult.Yes)
                {
                    m_twitchFile.ResetFile();
                    ParseTwitchFile();

                    // Ensure all options are disabled in twitch tab again
                    TwitchTabHandleAgreed();
                }

                Init();

                MessageBox.Show("Config has been reverted to default settings!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private void twitch_user_agreed_Clicked(object sender, RoutedEventArgs e)
        {
            TwitchTabHandleAgreed();
        }

        private void effect_user_config_Click(object sender, RoutedEventArgs e)
        {
            TreeMenuItem curTreeMenuItem = (TreeMenuItem)((TreeViewItem)((Grid)((Border)((ContentPresenter)((StackPanel)((Button)sender).Parent).TemplatedParent).Parent).Parent).TemplatedParent).DataContext;

            EffectType effectType = EffectType._EFFECT_ENUM_MAX;
            foreach (var pair in m_treeMenuItemsMap)
            {
                if (pair.Value == curTreeMenuItem)
                {
                    effectType = pair.Key;

                    break;
                }
            }

            if (effectType != EffectType._EFFECT_ENUM_MAX)
            {
                EffectInfo effectInfo = EffectsMap[effectType];
                EffectData effectData = GetEffectData(effectType);

                EffectConfig effectConfig = new EffectConfig(effectData, effectInfo);
                effectConfig.ShowDialog();

                if (effectConfig.IsSaved)
                {
                    effectData.TimedType = effectConfig.effectconf_timer_type_enable.IsChecked.Value ? (EffectTimedType)effectConfig.effectconf_timer_type.SelectedIndex
                        : effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL;
                    effectData.CustomTime = effectConfig.effectconf_timer_time_enable.IsChecked.Value
                        ? effectConfig.effectconf_timer_time.Text.Length > 0 ? int.Parse(effectConfig.effectconf_timer_time.Text) : -1 : -1;
                    effectData.Permanent = effectConfig.effectconf_timer_permanent_enable.IsChecked.Value;
                    effectData.WeightMult = effectConfig.effectconf_effect_weight_mult.SelectedIndex + 1;
                    effectData.ExcludedFromVoting = effectConfig.effectconf_exclude_voting_enable.IsChecked.Value;
                    effectData.CustomName = effectConfig.effectconf_effect_custom_name.Text.Trim();
                }
            }
        }

        private void contribute_modpage_click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://www.gta5-mods.com/scripts/chaos-mod-v-beta");
        }

        private void contribute_github_click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://github.com/gta-chaos-mod/ChaosModV");
        }

        private void contribute_donate_click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://paypal.me/EmrCue");
        }

        private void contribute_discord_click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://discord.gg/w2tDeKVaF9");
        }
    }
}