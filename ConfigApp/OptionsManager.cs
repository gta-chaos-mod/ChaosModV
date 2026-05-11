using System.IO;
using Shared;

namespace ConfigApp
{
    public static class OptionsManager
    {
        public static OptionsFile ConfigFile { get; } = new OptionsFile("configs/config.json", "configs/config.ini", "config.ini");
        public static OptionsFile VotingFile { get; } = new OptionsFile("configs/voting.json", "configs/voting.ini", "configs/twitch.ini", "twitch.ini");
        public static OptionsFile EffectsFile { get; } = new OptionsFile("configs/effects.json", "configs/effects.ini", "effects.ini");

        // These are written to manually
        public static OptionsFile WorkshopFile { get; } = new OptionsFile("configs/workshop.json", "configs/workshop.ini");

        private static IEnumerable<OptionsFile> ConfigFiles
        {
            get
            {
                yield return ConfigFile;
                yield return VotingFile;
                yield return EffectsFile;
                yield return WorkshopFile;
            }
        }

        public static void ReadFiles()
        {
            foreach (var file in ConfigFiles)
                file.ReadFile();
        }

        public static void WriteFiles()
        {
            ConfigFile.WriteFile();
            VotingFile.WriteFile();
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
            foreach (var file in new[] { ConfigFile, VotingFile, EffectsFile })
                DeleteFiles(file.CompatFilePaths);
        }

        private static void DeleteFiles(IEnumerable<string> files)
        {
            foreach (var file in files)
                File.Delete(file);
        }
    }
}
