using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ConfigApp.Workshop
{
    internal sealed class WorkshopSubmissionFileHandler
    {
        private const string CorruptSettingsMessage = "Submission settings file is corrupt, assuming default settings!";

        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public string SubmissionDirectory => $"workshop/{m_SubmissionItem.Id}/";
        private string SubmissionSettingsFile => $"workshop/{m_SubmissionItem.Id}.json";

        private readonly List<WorkshopSubmissionFile> m_Files;

        public WorkshopSubmissionFileHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
            m_Files = new List<WorkshopSubmissionFile>();
            ReloadFiles();
        }

        public List<WorkshopSubmissionFile> GetSubmissionFiles()
        {
            return m_Files;
        }

        internal void SetSettings(List<WorkshopSubmissionFileState> states)
        {
            var disabledFiles = new JArray();
            var effectSettings = new JObject();

            foreach (var state in states)
            {
                if (!state.Item.IsChecked)
                    disabledFiles.Add(state.FullPath);

                if (state.EffectData is not null && !IsDefaultEffectData(state.EffectData))
                    effectSettings[state.FullPath] = CreateEffectDataJson(state.EffectData);
            }

            var json = new JObject();
            if (disabledFiles.Count > 0)
                json["disabled_files"] = disabledFiles;
            if (effectSettings.Count > 0)
                json["effect_settings"] = effectSettings;

            if (json.Count == 0)
                DeleteSettingsFile();
            else
                File.WriteAllText(SubmissionSettingsFile, json.ToString());
        }

        public void ReloadFiles()
        {
            m_Files.Clear();

            if (!Directory.Exists(SubmissionDirectory))
                return;

            var settings = LoadSettings();
            foreach (var file in Directory.EnumerateFiles(SubmissionDirectory, "*", SearchOption.AllDirectories))
            {
                var pathName = file.Replace(SubmissionDirectory, string.Empty);
                m_Files.Add(new WorkshopSubmissionFile(pathName, !settings.DisabledFiles.Contains(pathName), settings.EffectSettings.GetValueOrDefault(pathName)));
            }
        }

        private (HashSet<string> DisabledFiles, Dictionary<string, EffectData> EffectSettings) LoadSettings()
        {
            var disabledFiles = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            var effectSettings = new Dictionary<string, EffectData>(StringComparer.OrdinalIgnoreCase);

            if (!File.Exists(SubmissionSettingsFile))
                return (disabledFiles, effectSettings);

            try
            {
                var fileText = File.ReadAllText(SubmissionSettingsFile);
                if (string.IsNullOrWhiteSpace(fileText))
                    return (disabledFiles, effectSettings);

                var json = JObject.Parse(fileText);
                LoadDisabledFiles(json, disabledFiles);
                LoadEffectSettings(json, effectSettings);
                return (disabledFiles, effectSettings);
            }
            catch (JsonReaderException ex)
            {
                throw CreateCorruptSettingsException(ex);
            }
            catch (JsonSerializationException ex)
            {
                throw CreateCorruptSettingsException(ex);
            }
        }

        private static Exception CreateCorruptSettingsException(Exception innerException)
        {
            return new Exception(CorruptSettingsMessage, innerException);
        }

        private static void LoadDisabledFiles(JObject json, ISet<string> disabledFiles)
        {
            if (!json.TryGetValue("disabled_files", out var disabledFilesToken) || disabledFilesToken is null)
                return;

            foreach (var file in disabledFilesToken.Select(file => file.Value<string>() ?? string.Empty))
            {
                if (!string.IsNullOrWhiteSpace(file))
                    disabledFiles.Add(file);
            }
        }

        private static void LoadEffectSettings(JObject json, IDictionary<string, EffectData> effectSettings)
        {
            if (!json.TryGetValue("effect_settings", out var effectSettingsToken) || effectSettingsToken is null)
                return;

            if (effectSettingsToken is not JObject settingsObject)
                return;

            foreach (var setting in settingsObject)
            {
                if (setting.Value is null || setting.Value.Type != JTokenType.Object)
                    continue;

                effectSettings[setting.Key] = Utils.ValueObjectToEffectData((JObject)setting.Value);
            }
        }

        private static bool IsDefaultEffectData(EffectData effectData)
        {
            return effectData.Enabled is null
                && effectData.TimedType is null
                && effectData.CustomTime is null
                && effectData.WeightMult is null
                && effectData.ExcludedFromVoting is null
                && effectData.CustomName is null
                && effectData.ShortcutKeycode is null;
        }

        private static JObject CreateEffectDataJson(EffectData effectData)
        {
            var json = new JObject();

            if (effectData.Enabled is not null)
                json["Enabled"] = new JValue(effectData.Enabled.Value ? 1 : 0);
            if (effectData.TimedType is not null)
                json["TimedType"] = new JValue((int)effectData.TimedType.Value);
            if (effectData.CustomTime is not null)
                json["CustomTime"] = new JValue(effectData.CustomTime.Value);
            if (effectData.WeightMult is not null)
                json["WeightMult"] = new JValue(effectData.WeightMult.Value);
            if (effectData.ExcludedFromVoting is not null)
                json["ExcludedFromVoting"] = new JValue(effectData.ExcludedFromVoting.Value ? 1 : 0);
            if (effectData.CustomName is not null)
                json["CustomName"] = new JValue(effectData.CustomName);
            if (effectData.ShortcutKeycode is not null)
                json["ShortcutKeycode"] = new JValue(effectData.ShortcutKeycode.Value);

            return json;
        }

        private void DeleteSettingsFile()
        {
            if (File.Exists(SubmissionSettingsFile))
                File.Delete(SubmissionSettingsFile);
        }
    }
}
