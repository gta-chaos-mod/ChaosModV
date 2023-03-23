using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Input;

namespace ConfigApp
{
    public class WorkshopSettingsHandler : ICommand
    {
        public event EventHandler CanExecuteChanged;

        private WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopSettingsHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object parameter)
        {
            return true;
        }

        public void Execute(object parameter)
        {
            var id = m_SubmissionItem.Id;
            var submissionDir = $"workshop/{id}/";
            if (!Directory.Exists(submissionDir))
            {
                return;
            }

            var submissionSettingsFile = $"workshop/{id}.json";
            var disabledFiles = new List<string>();
            if (File.Exists(submissionSettingsFile))
            {
                try
                {
                    var json = JObject.Parse(File.ReadAllText(submissionSettingsFile));
                    foreach (var file in json["disabled_files"])
                    {
                        disabledFiles.Add(file.Value<string>());
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
                var pathName = file.Replace($"{submissionDir}sounds\\", "").Replace(submissionDir, "");
                files.Add(new WorkshopSubmissionFile(pathName, !disabledFiles.Contains(pathName)));
            }

            var editWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Edit);
            if (!(bool)editWindow.ShowDialog())
            {
                return;
            }

            var disabledFilesArrayJson = new JArray();
            foreach (var state in editWindow.FileStates)
            {
                if (!state.Item.IsChecked)
                {
                    disabledFilesArrayJson.Add(state.FullPath);
                }
            }

            var disabledFilesJson = new JObject(new JProperty("disabled_files", disabledFilesArrayJson));
            File.WriteAllText(submissionSettingsFile, disabledFilesJson.ToString());
        }
    }
}
