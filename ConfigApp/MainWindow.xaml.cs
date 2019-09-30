using System.IO;
using System.Windows;
using System.Windows.Input;

namespace ConfigApp
{
    public partial class MainWindow : Window
    {
        const string ConfigFile = "config.ini";

        public MainWindow()
        {
            InitializeComponent();

            while (!File.Exists(ConfigFile) || !ParseConfigFile())
            {
                GenerateNewConfigFile();
            }
        }

        private void GenerateNewConfigFile()
        {
            string data = "";
            data += "NewEffectSpawnTime=60\n";
            data += "EffectTimedDur=180\n";
            data += "SpawnTimedEffects=1\n";

            File.WriteAllText(ConfigFile, data);
        }

        private bool ParseConfigFile()
        {
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

                int value;
                if (!int.TryParse(keyValue[1], out value))
                {
                    return false;
                }

                switch (keyValue[0])
                {
                    case "NewEffectSpawnTime":
                        user_effects_spawn_dur.Text = $"{value}";
                        break;
                    case "EffectTimedDur":
                        user_effects_timed_dur.Text = $"{value}";
                        break;
                    case "SpawnTimedEffects":
                        user_effects_allow_timed.IsChecked = value != 0;
                        break;
                }
            }

            return true;
        }

        private void WriteConfigFile()
        {
            string data = "";
            data += $"NewEffectSpawnTime={(user_effects_spawn_dur.Text.Length > 0 ? user_effects_spawn_dur.Text : "60")}\n";
            data += $"EffectTimedDur={(user_effects_timed_dur.Text.Length > 0 ? user_effects_timed_dur.Text : "180")}\n";
            data += $"SpawnTimedEffects={(user_effects_allow_timed.IsChecked.Value ? "1" : "0")}\n";
            File.WriteAllText(ConfigFile, data);
        }

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!char.IsDigit(e.Text[0]))
            {
                e.Handled = true;
            }
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
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
            MessageBox.Show("Saved Config!");
        }
    }
}
