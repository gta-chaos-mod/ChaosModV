using System;
using System.Net.Http;
using System.Windows.Input;
using System.Windows;
using System.IO;
using System.Windows.Controls;
using System.Collections.Generic;
using System.IO.Compression;
using System.Security.Cryptography;
using Newtonsoft.Json.Linq;
using System.Text;

namespace ConfigApp
{
    public class WorkshopInstallHandler : ICommand
    {
        public event EventHandler CanExecuteChanged;

        private WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopInstallHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object parameter)
        {
            return m_SubmissionItem.InstallState != WorkshopSubmissionItem.SubmissionInstallState.Installing
                && m_SubmissionItem.InstallState != WorkshopSubmissionItem.SubmissionInstallState.Removing;
        }

        public async void Execute(object parameter)
        {
            if (!Directory.Exists("scripts/"))
            {
                MessageBox.Show("\"scripts\" folder not found, is this the chaosmod directory? Otherwise create the folder and try again.", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            var fatalCleanup = new Action(() =>
            {
                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.NotInstalled;
            });

            var targetDirName = $"scripts/workshop/{m_SubmissionItem.Id}";

            if (m_SubmissionItem.InstallState == WorkshopSubmissionItem.SubmissionInstallState.Installed)
            {
                // Remove submission
                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Removing;

                try
                {
                    Directory.Delete(targetDirName, true);
                }
                catch (DirectoryNotFoundException)
                {

                }
                catch (IOException)
                {
                    MessageBox.Show($"Couldn't access \"{targetDirName}\". Please delete that directory and try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                    fatalCleanup();
                    return;
                }

                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.NotInstalled;
                return;
            }

            m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installing;

            HttpClient httpClient = new HttpClient();
            try
            {
                var result = await httpClient.GetAsync($"http://chaos.gopong.dev/workshop/fetch_submission?submission_id={m_SubmissionItem.Id}");
                if (!result.IsSuccessStatusCode)
                {
                    MessageBox.Show("Error while fetching submission. Please try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                    fatalCleanup();
                    return;
                }

                var fileStream = await result.Content.ReadAsStreamAsync();

                var sha256StrBuilder = new StringBuilder();
                using (var hash = SHA256.Create())
                {
                    var resultHash = hash.ComputeHash(fileStream);
                    foreach (var b in resultHash)
                    {
                        sha256StrBuilder.Append(b.ToString("x2"));
                    }
                }
                if (sha256StrBuilder.ToString() != m_SubmissionItem.Sha256)
                {
                    MessageBox.Show("SHA256 mismatch! Please try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                    fatalCleanup();
                    return;
                }

                using (ZipArchive archive = new ZipArchive(fileStream))
                {
                    // Try deleting the directory first if it exists, otherwise extraction might fail if it hits a duplicate file
                    try
                    {
                        Directory.Delete(targetDirName, true);
                    }
                    catch (DirectoryNotFoundException)
                    {

                    }
                    catch (IOException)
                    {
                        MessageBox.Show($"Couldn't access \"{targetDirName}\". Please delete that directory and try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                        fatalCleanup();
                        return;
                    }

                    archive.ExtractToDirectory(targetDirName);
                }

                JObject metadataJson = new JObject();
                metadataJson["name"] = m_SubmissionItem.Name;
                metadataJson["description"] = m_SubmissionItem.Description;
                metadataJson["version"] = m_SubmissionItem.Version;
                metadataJson["lastupdated"] = m_SubmissionItem.LastUpdated;
                metadataJson["sha256"] = m_SubmissionItem.Sha256;
                File.WriteAllText($"{targetDirName}/metadata.json", metadataJson.ToString());

                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
            }
            catch (HttpRequestException)
            {
                MessageBox.Show("Error while fetching submission. Please try again!", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                fatalCleanup();
                return;
            }
        }
    }
}
