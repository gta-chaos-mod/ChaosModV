using System.IO;
using Shared;

namespace ConfigApp
{
    public static class OptionsManager
    {
        public static OptionsFile ConfigFile { get; } = new("configs/config.json", "configs/config.ini", "config.ini");
        public static OptionsFile VotingFile { get; } = new("configs/voting.json", "configs/voting.ini", "configs/twitch.ini", "twitch.ini");
        public static OptionsFile EffectsFile { get; } = new("configs/effects.json", "configs/effects.ini", "effects.ini");
        public static OptionsFile WorkshopFile { get; } = new("configs/workshop.json", "configs/workshop.ini");

        public static void ReadFiles()
        {
            foreach (var file in GetAllFiles())
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
            ConfigFile.ResetFile();
            EffectsFile.ResetFile();
        }

        public static void DeleteCompatFiles()
        {
            DeleteFiles(ConfigFile.CompatFilePaths);
            DeleteFiles(VotingFile.CompatFilePaths);
            DeleteFiles(EffectsFile.CompatFilePaths);
        }

        private static IEnumerable<OptionsFile> GetAllFiles()
        {
            yield return ConfigFile;
            yield return VotingFile;
            yield return EffectsFile;
            yield return WorkshopFile;
        }

        private static void DeleteFiles(IEnumerable<string> files)
        {
            foreach (var file in files)
                File.Delete(file);
        }
    }
}
