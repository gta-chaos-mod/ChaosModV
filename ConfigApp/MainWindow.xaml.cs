using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using static ConfigApp.Effects;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        public TreeMenuItem(string text, TreeMenuItem parent = null)
        {
            Text = text;
            Parent = parent;
            Children = new List<TreeMenuItem>();
            _IsChecked = true;
        }

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

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));

                foreach (TreeMenuItem menuItem in Children)
                {
                    menuItem.IsChecked = value;
                }

                Parent?.UpdateCheckedAccordingToChildrenStatus();
            }
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

            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));
        }
    }

    public partial class MainWindow : Window
    {
        private const string ConfigFile = "config.ini";
        private const string EffectsFile = "effects.ini";

        private Dictionary<EffectType, TreeMenuItem> TreeMenuItemsMap;

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

            if (File.Exists(".twitchmode"))
            {
                twitch_tab.Visibility = Visibility.Visible;
            }
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

            bool lazyFoundAll = false;
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
                            lazyFoundAll = true;
                            misc_user_effects_effect_timer_color.SelectedColor = (Color)ColorConverter.ConvertFromString(keyValue[1]);
                            break;
                    }
                }
            }

            if (!lazyFoundAll)
            {
                MessageBox.Show("Your config file was incomplete and thus has been regenerated.", "ChaosModV",
                    MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
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

                if (!int.TryParse(keyValue[1], out int value))
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

                if (effectType != EffectType._EFFECT_ENUM_MAX)
                {
                    TreeMenuItemsMap[effectType].IsChecked = value != 0 ? true : false;
                }
            }

            return true;
        }

        private void WriteEffectsFile()
        {
            string data = "";

            for (EffectType effectType = 0; effectType < EffectType._EFFECT_ENUM_MAX; effectType++)
            {
                data += $"{EffectsMap[effectType].Id}={(TreeMenuItemsMap[effectType].IsChecked ? 1 : 0)}\n";
            }

            File.WriteAllText(EffectsFile, data);
        }

        private void InitEffectsTreeView()
        {
            TreeMenuItemsMap = new Dictionary<EffectType, TreeMenuItem>();

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

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!char.IsDigit(e.Text[0]))
            {
                e.Handled = true;
            }
        }

        private void NoSpacePreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }

        private void NoCopyPastePreviewExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            if (e.Command == ApplicationCommands.Copy || e.Command == ApplicationCommands.Cut || e.Command == ApplicationCommands.Paste)
            {
                e.Handled = true;
            }
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
    }
}