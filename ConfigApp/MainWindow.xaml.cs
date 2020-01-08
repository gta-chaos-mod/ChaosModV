using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using static ConfigApp.Effects;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        public string Text { get; private set; }
        public TreeMenuItem Parent;
        public List<TreeMenuItem> Children { get; private set; }
        public event PropertyChangedEventHandler PropertyChanged;
        private bool _IsChecked;
        public bool IsChecked
        {
            get
            {
                return _IsChecked;
            }
            set
            {
                _IsChecked = value;

                _NotifyFieldsUpdated();

                foreach (TreeMenuItem menuItem in Children)
                {
                    menuItem.IsChecked = value;
                }

                Parent?.UpdateCheckedAccordingToChildrenStatus();
            }
        }
        public string IsConfigVisible
        {
            get
            {
                return Children.Count == 0 ? "Visible" : "Hidden";
            }
        }
        public bool IsConfigEnabled
        {
            get
            {
                return IsChecked;
            }
        }

        public TreeMenuItem(string text, TreeMenuItem parent = null)
        {
            Text = text;
            Parent = parent;
            Children = new List<TreeMenuItem>();
            _IsChecked = true;
        }

        public void AddChild(TreeMenuItem menuItem)
        {
            if (menuItem != null)
            {
                menuItem.Parent = this;
                Children.Add(menuItem);
            }
        }

        public void UpdateCheckedAccordingToChildrenStatus()
        {
            bool shouldBeChecked = false;
            foreach (TreeMenuItem menuItem in Children)
            {
                if (menuItem.IsChecked)
                {
                    shouldBeChecked = true;
                    break;
                }
            }

            _IsChecked = shouldBeChecked;

            _NotifyFieldsUpdated();
        }

        private void _NotifyFieldsUpdated()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigVisible"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigEnabled"));
        }
    }

    public class EffectData
    {
        public EffectTimedType EffectTimedType;
        public int EffectCustomTime;
        public int EffectWeight;

        public EffectData(EffectTimedType effectTimedType, int effectCustomTime, int effectWeight)
        {
            EffectTimedType = effectTimedType;
            EffectCustomTime = effectCustomTime;
            EffectWeight = effectWeight;
        }
    }

    public partial class MainWindow : Window
    {
        private const string ConfigFile = "config.ini";
        private const string EffectsFile = "effects.ini";

        private Dictionary<EffectType, TreeMenuItem> TreeMenuItemsMap;
        private Dictionary<EffectType, EffectData> EffectDataMap;

        public MainWindow()
        {
            InitializeComponent();

            while (!ParseConfigFile())
            {
                WriteConfigFile();
            }

            InitEffectsTreeView();

            while (!ParseEffectsFile())
            {
                WriteEffectsFile();
            }

            InitTwitchTab();
        }

        private bool ParseConfigFile()
        {
            if (!File.Exists(ConfigFile))
            {
                return false;
            }

            string data = File.ReadAllText(ConfigFile);
            if (data.Length == 0)
            {
                return false;
            }

            foreach (string line in data.Split('\n'))
            {
                string[] keyValue = line.Split('=');
                if (keyValue.Length != 2)
                {
                    continue;
                }

                string key = keyValue[0];

                if (int.TryParse(keyValue[1], out int value))
                {
                    switch (key)
                    {
                        case "NewEffectSpawnTime":
                            misc_user_effects_spawn_dur.Text = $"{value}";
                            break;
                        case "EffectTimedDur":
                            misc_user_effects_timed_dur.Text = $"{value}";
                            break;
                        case "Seed":
                            if (value >= 0)
                            {
                                misc_user_effects_random_seed.Text = $"{value}";
                            }
                            break;
                        case "EffectTimedShortDur":
                            misc_user_effects_timed_short_dur.Text = $"{value}";
                            break;
                        case "EnableTwitchVoting":
                            twitch_user_agreed.IsChecked = value != 0;
                            break;
                        case "TwitchVotingNoVoteChance":
                            twitch_user_effects_chance_no_voting_round.Text = $"{(value >= 0 ? value <= 100 ? value : 100 : 0)}";
                            break;
                        case "TwitchVotingSecsBeforeVoting":
                            twitch_user_effects_secs_before_chat_voting.Text = $"{(value >= 0 ? value == 1 ? 2 : value : 0)}";
                            break;
                        case "EnableClearEffectsShortcut":
                            misc_user_effects_clear_enable.IsChecked = value != 0;
                            break;
                        case "DisableEffectTwiceInRow":
                            misc_user_effects_twice_disable.IsChecked = value != 0;
                            break;
                        case "DisableTimerBarDraw":
                            misc_user_effects_drawtimer_disable.IsChecked = value != 0;
                            break;
                        case "DisableEffectTextDraw":
                            misc_user_effects_drawtext_disable.IsChecked = value != 0;
                            break;
                        case "EnableToggleModShortcut":
                            misc_user_toggle_mod_shortcut.IsChecked = value != 0;
                            break;
                    }
                }
                else
                {
                    switch (key)
                    {
                        case "EffectTimerColor":
                            misc_user_effects_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(keyValue[1]);
                            break;
                        case "EffectTextColor":
                            misc_user_effects_text_color.SelectedColor = (Color)ColorConverter.ConvertFromString(keyValue[1]);
                            break;
                        case "EffectTimedTimerColor":
                            misc_user_effects_effect_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(keyValue[1]);
                            break;
                        case "TwitchChannelName":
                            twitch_user_channel_name.Text = keyValue[1];
                            break;
                        case "TwitchUserName":
                            twitch_user_user_name.Text = keyValue[1];
                            break;
                        case "TwitchChannelOAuth":
                            twitch_user_channel_oauth.Text = keyValue[1];
                            break;
                    }
                }
            }

            return true;
        }

        private void WriteConfigFile()
        {
            string data = "";

            data += $"NewEffectSpawnTime={(misc_user_effects_spawn_dur.Text.Length > 0 ? misc_user_effects_spawn_dur.Text : "30")}\n";
            data += $"EffectTimedDur={(misc_user_effects_timed_dur.Text.Length > 0 ? misc_user_effects_timed_dur.Text : "90")}\n";
            data += $"Seed={(misc_user_effects_random_seed.Text.Length > 0 ? misc_user_effects_random_seed.Text : "-1")}\n";
            data += $"EffectTimedShortDur={(misc_user_effects_timed_short_dur.Text.Length > 0 ? misc_user_effects_timed_short_dur.Text : "30")}\n";
            data += $"EnableTwitchVoting={(twitch_user_agreed.IsChecked.Value ? "1" : "0")}\n";
            data += $"TwitchChannelName={(twitch_user_channel_name.Text)}\n";
            data += $"TwitchUserName={(twitch_user_user_name.Text)}\n";
            data += $"TwitchChannelOAuth={(twitch_user_channel_oauth.Text)}\n";
            data += $"TwitchVotingNoVoteChance={(twitch_user_effects_chance_no_voting_round.Text != null ? twitch_user_effects_chance_no_voting_round.Text : "5")}\n";
            data += $"TwitchVotingSecsBeforeVoting={(twitch_user_effects_secs_before_chat_voting.Text.Length > 0 ? twitch_user_effects_secs_before_chat_voting.Text : "0")}\n";
            data += $"EnableClearEffectsShortcut={(misc_user_effects_clear_enable.IsChecked.Value ? "1" : "0")}\n";
            data += $"DisableEffectTwiceInRow={(misc_user_effects_twice_disable.IsChecked.Value ? "1" : "0")}\n";
            data += $"DisableTimerBarDraw={(misc_user_effects_drawtimer_disable.IsChecked.Value ? "1" : "0")}\n";
            data += $"DisableEffectTextDraw={(misc_user_effects_drawtext_disable.IsChecked.Value ? "1" : "0")}\n";
            data += $"EnableToggleModShortcut={(misc_user_toggle_mod_shortcut.IsChecked.Value ? "1" : "0")}\n";
            data += $"EffectTimerColor={(misc_user_effects_timer_color.SelectedColor)}\n";
            data += $"EffectTextColor={(misc_user_effects_text_color.SelectedColor)}\n";
            data += $"EffectTimedTimerColor={(misc_user_effects_effect_timer_color.SelectedColor)}\n";

            File.WriteAllText(ConfigFile, data);
        }

        private bool ParseEffectsFile()
        {
            if (!File.Exists(EffectsFile))
            {
                return false;
            }

            string data = File.ReadAllText(EffectsFile);
            if (data.Length == 0)
            {
                return false;
            }

            foreach (string line in data.Split('\n'))
            {
                string[] keyValue = line.Split('=');
                if (keyValue.Length != 2)
                {
                    continue;
                }

                string key = keyValue[0];
                string value = keyValue[1];

                string[] values = value.Split(',');

                if (!int.TryParse(values[0], out int enabled))
                {
                    return false;
                }

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

                if (values.Length >= 4)
                {
                    if (!Enum.TryParse(values[1], out effectTimedType))
                    {
                        return false;
                    }
                    if (!int.TryParse(values[2], out effectTimedTime))
                    {
                        return false;
                    }
                    if (!int.TryParse(values[3], out effectWeight))
                    {
                        return false;
                    }
                }

                if (effectType != EffectType._EFFECT_ENUM_MAX)
                {
                    TreeMenuItemsMap[effectType].IsChecked = enabled == 0 ? false : true;

                    EffectDataMap.Add(effectType, new EffectData(effectTimedType, effectTimedTime, effectWeight));
                }
            }

            return true;
        }

        private void WriteEffectsFile()
        {
            string data = "";

            for (EffectType effectType = 0; effectType < EffectType._EFFECT_ENUM_MAX; effectType++)
            {
                EffectInfo effectInfo = EffectsMap[effectType];

                EffectDataMap.TryGetValue(effectType, out EffectData effectData);
                if (effectData == null)
                {
                    effectData = new EffectData(effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL, -1, 5);
                }

                data += $"{EffectsMap[effectType].Id}={(TreeMenuItemsMap[effectType].IsChecked ? 1 : 0)}" +
                    $",{(effectData.EffectTimedType == EffectTimedType.TIMED_NORMAL ? 0 : 1)},{effectData.EffectCustomTime},{effectData.EffectWeight}\n";
            }

            File.WriteAllText(EffectsFile, data);
        }

        private void InitEffectsTreeView()
        {
            TreeMenuItemsMap = new Dictionary<EffectType, TreeMenuItem>();
            EffectDataMap = new Dictionary<EffectType, EffectData>();

            TreeMenuItem playerParentItem = new TreeMenuItem("Player");
            TreeMenuItem vehicleParentItem = new TreeMenuItem("Vehicle");
            TreeMenuItem pedsParentItem = new TreeMenuItem("Peds");
            TreeMenuItem timeParentItem = new TreeMenuItem("Time");
            TreeMenuItem weatherParentItem = new TreeMenuItem("Weather");
            TreeMenuItem miscParentItem = new TreeMenuItem("Misc");

            for (EffectType effectType = 0; effectType < EffectType._EFFECT_ENUM_MAX; effectType++)
            {
                EffectInfo effectInfo = EffectsMap[effectType];
                string effectName = effectInfo.Name;
                TreeMenuItem menuItem = new TreeMenuItem(effectName);
                TreeMenuItemsMap.Add(effectType, menuItem);

                switch (effectInfo.EffectCategory)
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
                }
            }

            effects_user_effects_tree_view.Items.Add(playerParentItem);
            effects_user_effects_tree_view.Items.Add(vehicleParentItem);
            effects_user_effects_tree_view.Items.Add(pedsParentItem);
            effects_user_effects_tree_view.Items.Add(timeParentItem);
            effects_user_effects_tree_view.Items.Add(weatherParentItem);
            effects_user_effects_tree_view.Items.Add(miscParentItem);
        }

        void InitTwitchTab()
        {
            if (!File.Exists(".twitchmode"))
            {
                twitch_tab.Visibility = Visibility.Hidden;
            }

            TwitchTabHandleAgreed();
        }

        void TwitchTabHandleAgreed()
        {
            bool agreed = twitch_user_agreed.IsChecked.GetValueOrDefault();

            twitch_user_channel_name.IsEnabled = agreed;
            twitch_user_channel_oauth.IsEnabled = agreed;
            twitch_user_user_name.IsEnabled = agreed;
            twitch_user_effects_chance_no_voting_round.IsEnabled = agreed;
            twitch_user_effects_secs_before_chat_voting.IsEnabled = agreed;
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
            WriteEffectsFile();

            MessageBox.Show("Saved Config!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void user_reset_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Are you sure you want to reset your Config?", "ChaosModV",
                MessageBoxButton.YesNo, MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                File.WriteAllText(ConfigFile, "");
                File.WriteAllText(EffectsFile, "");

                MessageBox.Show("Resetted Config!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Information);

                System.Diagnostics.Process.Start(Application.ResourceAssembly.Location);
                Application.Current.Shutdown();
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
            foreach (var pair in TreeMenuItemsMap)
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
                EffectData effectData = EffectDataMap[effectType];

                EffectConfig effectConfig = new EffectConfig(effectInfo.IsTimed, effectData.EffectTimedType, effectInfo, effectData.EffectCustomTime, effectData.EffectWeight);
                effectConfig.Title = effectInfo.Name;
                effectConfig.ShowDialog();

                if (effectConfig.IsSaved)
                {
                    effectData.EffectTimedType = effectConfig.effectconf_timer_type_enable.IsChecked.Value ? (EffectTimedType)effectConfig.effectconf_timer_type.SelectedIndex
                        : effectInfo.IsShort ? EffectTimedType.TIMED_SHORT : EffectTimedType.TIMED_NORMAL;
                    effectData.EffectCustomTime = effectConfig.effectconf_timer_time_enable.IsChecked.Value
                        ? effectConfig.effectconf_timer_time.Text.Length > 0 ? int.Parse(effectConfig.effectconf_timer_time.Text) : -1 : -1;
                    effectData.EffectWeight = effectConfig.effectconf_effect_weight.SelectedIndex + 1;
                }
            }
        }
    }
}