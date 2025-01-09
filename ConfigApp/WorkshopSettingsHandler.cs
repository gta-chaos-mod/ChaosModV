using System.IO;
using System.Windows;
using System.Windows.Input;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ConfigApp
{
    public class WorkshopSettingsHandler : ICommand
    {
        public event EventHandler? CanExecuteChanged = null;

        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopSettingsHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object? parameter)
        {
            return true;
        }

        public void Execute(object? parameter)
        {
            var id = m_SubmissionItem.Id;
            var submissionDir = $"workshop/{id}/";
            if (!Directory.Exists(submissionDir))
            {
                return;
            }

            var submissionSettingsFile = $"workshop/{id}.json";
            var disabledFiles = new List<string>();
            var scriptSettings = new Dictionary<string, EffectData>();
            if (File.Exists(submissionSettingsFile))
            {
                try
                {
                    var fileText = File.ReadAllText(submissionSettingsFile);
                    if (!string.IsNullOrWhiteSpace(fileText))
                    {
                        var json = JObject.Parse(fileText);

                        if (json.ContainsKey("disabled_files"))
                        {
                            disabledFiles.AddRange(json["disabled_files"]?.Select(file => file.Value<string>() ?? string.Empty) ?? Array.Empty<string>());
                        }

                        if (json.ContainsKey("effect_settings"))
                        {
                            var scriptSettingsJson = json["effect_settings"]?.ToObject<Dictionary<string, EffectData>>();
                            if (scriptSettingsJson is not null)
                            {
                                scriptSettings = scriptSettingsJson;
                            }
                        }
                    }
                }
                catch (JsonException)
                {
                    MessageBox.Show($"Submission settings file is corrupt, assuming default settings!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }

            var files = new List<WorkshopSubmissionFile>();
            foreach (var file in Directory.EnumerateFiles(submissionDir, "*", SearchOption.AllDirectories))
            {
                var pathName = file.Replace(submissionDir, "");
                files.Add(new WorkshopSubmissionFile(pathName, !disabledFiles.Contains(pathName),
                    scriptSettings.ContainsKey(pathName) ? scriptSettings[pathName] : null));
            }

            var editWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Edit);
            editWindow.ShowDialog();

            var disabledFilesArrayJson = new JArray();
            var scriptSettingsObjectJson = new JObject();
            foreach (var state in editWindow.FileStates)
            {
                if (!state.Item.IsChecked)
                {
                    disabledFilesArrayJson.Add(state.FullPath);
                }

                if (state.EffectData != null)
                {
                    // Don't save settings if everything is set 1:1 as defaults
                    if (JsonConvert.SerializeObject(state.EffectData) != JsonConvert.SerializeObject(new EffectData()))
                    {
                        scriptSettingsObjectJson[state.FullPath] = JToken.FromObject(state.EffectData);
                    }
                }
            }

            var newJson = new JObject();
            if (disabledFilesArrayJson.Count > 0)
            {
                newJson.Add(new JProperty("disabled_files", disabledFilesArrayJson));
            }
            if (scriptSettingsObjectJson.Count > 0)
            {
                newJson.Add(new JProperty("effect_settings", scriptSettingsObjectJson));
            }

            if (newJson.Count == 0)
            {
                File.Delete(submissionSettingsFile);
            }
            else
            {
                File.WriteAllText(submissionSettingsFile, $"{newJson}");
            }
        }
    }
}
