using Shared;
using System.IO;

namespace ConfigApp
{
    public static class OptionsManager
    {
        public static OptionsFile ConfigFile { get; } = new OptionsFile("configs/config.ini", "config.ini");
        public static OptionsFile TwitchFile { get; } = new OptionsFile("configs/twitch.ini", "twitch.ini");
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
            if (ConfigFile.HasCompatFile())
            {
                File.Delete(ConfigFile.GetCompatFile());
            }
            if (TwitchFile.HasCompatFile())
            {
                File.Delete(TwitchFile.GetCompatFile());
            }
            if (EffectsFile.HasCompatFile())
            {
                File.Delete(EffectsFile.GetCompatFile());
            }
        }
    }
}
