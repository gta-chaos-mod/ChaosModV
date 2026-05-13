using System.IO;
using Shared;

namespace ConfigApp
{
    internal static class OptionsManager
    {
        public static OptionsFile ConfigFile { get; } = new("configs/config.json", "configs/config.ini", "config.ini");
        public static OptionsFile VotingFile { get; } = new("configs/voting.json", "configs/voting.ini", "configs/twitch.ini", "twitch.ini");
        public static OptionsFile EffectsFile { get; } = new("configs/effects.json", "configs/effects.ini", "effects.ini");
        public static OptionsFile WorkshopFile { get; } = new("configs/workshop.json", "configs/workshop.ini");

        public static void ReadFiles()
        {
            ConfigFile.ReadFile();
            VotingFile.ReadFile();
            EffectsFile.ReadFile();
            WorkshopFile.ReadFile();
        }

        public static void WriteFiles()
        {
            ConfigFile.WriteFile();
            VotingFile.WriteFile();
            EffectsFile.WriteFile();
        }

        public static void ResetFiles()
        {
            ConfigFile.ResetFile();
            EffectsFile.ResetFile();
        }

        public static void DeleteCompatFiles()
        {
            foreach (var file in ConfigFile.CompatFilePaths.Concat(VotingFile.CompatFilePaths).Concat(EffectsFile.CompatFilePaths))
                File.Delete(file);
        }
    }
}
