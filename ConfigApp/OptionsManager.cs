using Shared;
using System.IO;

namespace ConfigApp
{
    public static class OptionsManager
    {
        public static OptionsFile ConfigFile { get; } = new OptionsFile("configs/config.ini", "config.ini");
        public static OptionsFile TwitchFile { get; } = new OptionsFile("configs/voting.ini", "configs/twitch.ini", "twitch.ini");
        public static OptionsFile EffectsFile { get; } = new OptionsFile("configs/effects.ini", "effects.ini");

        public static void ReadFiles()
        {
            ConfigFile.ReadFile();
            TwitchFile.ReadFile();
            EffectsFile.ReadFile();
        }

        public static void WriteFiles()
        {
            ConfigFile.WriteFile();
            TwitchFile.WriteFile();
            EffectsFile.WriteFile();
        }

        public static void ResetFiles()
        {
            // Exclude TwitchFile as that one is reset separately

            ConfigFile.ResetFile();
            EffectsFile.ResetFile();
        }

        public static void DeleteCompatFiles()
        {
            void deleteFiles(string[] files)
            {
                foreach (var file in files)
                {
                    File.Delete(file);
                }
            }

            if (ConfigFile.HasCompatFile())
            {
                deleteFiles(ConfigFile.GetCompatFiles());
            }
            if (TwitchFile.HasCompatFile())
            {
                deleteFiles(TwitchFile.GetCompatFiles());
            }
            if (EffectsFile.HasCompatFile())
            {
                deleteFiles(EffectsFile.GetCompatFiles());
            }
        }
    }
}
