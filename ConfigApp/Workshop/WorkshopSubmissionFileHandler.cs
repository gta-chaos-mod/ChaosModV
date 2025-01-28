namespace ConfigApp.Workshop
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Newtonsoft.Json.Linq;
    using System.Windows;
    using Newtonsoft.Json;

    public class WorkshopSubmissionFileHandler
    {
        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public String SubmissionDirectory => $"workshop/{m_SubmissionItem.Id}/";
        private String SubmissionSettingsFile => $"workshop/{m_SubmissionItem.Id}.json";

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

        public void SetSettings(List<WorkshopSubmissionFileState> states)
        {
            var disabledFilesArrayJson = new JArray();
            var scriptSettingsObjectJson = new JObject();
            foreach (var state in states)
            {
                if (!state.Item.IsChecked)
                    disabledFilesArrayJson.Add(state.FullPath);

                if (state.EffectData != null)
                    // Don't save settings if everything is set 1:1 as defaults
                    if (JsonConvert.SerializeObject(state.EffectData) != JsonConvert.SerializeObject(new EffectData()))
                        scriptSettingsObjectJson[state.FullPath] = JToken.FromObject(state.EffectData);
            }

            var newJson = new JObject();
            if (disabledFilesArrayJson.Count > 0)
                newJson.Add(new JProperty("disabled_files", disabledFilesArrayJson));
            if (scriptSettingsObjectJson.Count > 0)
                newJson.Add(new JProperty("effect_settings", scriptSettingsObjectJson));

            if (newJson.Count == 0)
                File.Delete(SubmissionSettingsFile);
            else
                File.WriteAllText(SubmissionSettingsFile, $"{newJson}");
        }

        public void ReloadFiles()
        {
            m_Files.Clear();

            if (Directory.Exists(SubmissionDirectory))
            {
                var disabledFiles = new List<string>();
                var scriptSettings = new Dictionary<string, EffectData>();
                if (File.Exists(SubmissionSettingsFile))
                    try
                    {
                        var fileText = File.ReadAllText(SubmissionSettingsFile);
                        if (!string.IsNullOrWhiteSpace(fileText))
                        {
                            var json = JObject.Parse(fileText);

                            if (json.ContainsKey("disabled_files"))
                                disabledFiles.AddRange(json["disabled_files"]?.Select(file => file.Value<string>() ?? string.Empty) ?? Array.Empty<string>());

                            if (json.ContainsKey("effect_settings"))
                            {
                                var scriptSettingsJson = json["effect_settings"]?.ToObject<Dictionary<string, EffectData>>();
                                if (scriptSettingsJson is not null)
                                    scriptSettings = scriptSettingsJson;
                            }
                        }
                    }
                    catch (JsonException)
                    {
                        throw new Exception("Submission settings file is corrupt, assuming default settings!");
                    }

                foreach (var file in Directory.EnumerateFiles(SubmissionDirectory, "*", SearchOption.AllDirectories))
                {
                    var pathName = file.Replace(SubmissionDirectory, "");
                    m_Files.Add(new WorkshopSubmissionFile(pathName, !disabledFiles.Contains(pathName),
                        scriptSettings.ContainsKey(pathName) ? scriptSettings[pathName] : null));
                }
            }
        }
    }
}
