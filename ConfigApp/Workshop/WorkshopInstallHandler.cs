using System.IO;
using System.IO.Compression;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using System.Windows.Input;
using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json.Linq;
using ZstdSharp;

namespace ConfigApp.Workshop
{
    public class WorkshopInstallHandler : ICommand
    {
        private const string WorkshopDirectory = "workshop";
        private const string WorkshopCacheDirectory = "workshopcache";

        public event EventHandler? CanExecuteChanged
        {
            add { }
            remove { }
        }

        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopInstallHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object? parameter)
        {
            return m_SubmissionItem.InstallState is not WorkshopSubmissionItem.SubmissionInstallState.Installing
                and not WorkshopSubmissionItem.SubmissionInstallState.Removing;
        }

        public async void Execute(object? parameter)
        {
            Directory.CreateDirectory(WorkshopDirectory);

            var originalInstallState = m_SubmissionItem.InstallState;
            var targetDirName = Path.Combine(WorkshopDirectory, m_SubmissionItem.Id ?? string.Empty);

            if (originalInstallState == WorkshopSubmissionItem.SubmissionInstallState.Installed)
            {
                await RemoveSubmissionAsync(targetDirName, originalInstallState);
                return;
            }

            if (!IsValidSubmissionId())
            {
                await AppDialog.ShowMessageAsync("Invalid submission id! Refusing to install.");
                RestoreInstallState(originalInstallState);
                return;
            }

            m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installing;

            try
            {
                var fileContent = await LoadSubmissionContentAsync();
                if (fileContent is null)
                {
                    RestoreInstallState(originalInstallState);
                    return;
                }

                using var archiveStream = GetArchiveStream(fileContent);
                if (!await ConfirmAndExtractAsync(archiveStream, targetDirName, originalInstallState))
                    return;

                WriteMetadata(targetDirName);
                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;
                Console.Beep();
            }
            catch (HttpRequestException)
            {
                await AppDialog.ShowMessageAsync("Error while fetching submission. Submission might have been removed by remote. Please refresh and try again!");
                RestoreInstallState(originalInstallState);
            }
        }

        private bool IsValidSubmissionId()
        {
            return m_SubmissionItem.Id is not null
                && m_SubmissionItem.Id.All(c => char.IsLower(c) || char.IsDigit(c));
        }

        private async Task RemoveSubmissionAsync(string targetDirName, WorkshopSubmissionItem.SubmissionInstallState originalInstallState)
        {
            m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Removing;

            if (!await AppDialog.ShowYesNoAsync("Are you sure you want to remove this submission?"))
            {
                RestoreInstallState(originalInstallState);
                return;
            }

            if (!TryDeleteDirectory(targetDirName, originalInstallState))
                return;

            try
            {
                File.Delete($"{targetDirName}.json");
            }
            catch (FileNotFoundException) { }

            m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.NotInstalled;
            Console.Beep();
        }

        private async Task<byte[]?> LoadSubmissionContentAsync()
        {
            var cacheFilePath = GetCacheFilePath();
            if (File.Exists(cacheFilePath))
            {
                var cachedContent = File.ReadAllBytes(cacheFilePath);
                if (GetFileSha256(cachedContent) == m_SubmissionItem.Sha256)
                    return cachedContent;
            }

            using var httpClient = new HttpClient();
            var domain = OptionsManager.WorkshopFile.ReadValue("WorkshopCustomUrl", Info.WORKSHOP_DEFAULT_URL);
            var result = await httpClient.GetAsync($"{domain}/workshop/fetch_submission_data?submission_id={m_SubmissionItem.Id}");

            if (!result.IsSuccessStatusCode)
            {
                await AppDialog.ShowMessageAsync("Error while fetching submission. Please try again!");
                return null;
            }

            var fileContent = await result.Content.ReadAsByteArrayAsync();
            if (GetFileSha256(fileContent) != m_SubmissionItem.Sha256)
            {
                await AppDialog.ShowMessageAsync("SHA256 mismatch! Please refresh submissions and try again!");
                return null;
            }

            TryCacheSubmission(cacheFilePath, fileContent, result.Headers.Contains("Compressed") && result.Headers.GetValues("Compressed").Contains("yes"));
            return fileContent;
        }

        private string GetCacheFilePath()
        {
            Directory.CreateDirectory(WorkshopCacheDirectory);
            return Path.Combine(WorkshopCacheDirectory, $"{m_SubmissionItem.Id}.zip.zst");
        }

        private static void TryCacheSubmission(string cacheFilePath, byte[] fileContent, bool isFileCompressed)
        {
            try
            {
                var cachedFileContent = fileContent;
                if (!isFileCompressed)
                {
                    var compressor = new Compressor(10);
                    cachedFileContent = compressor.Wrap(fileContent).ToArray();
                }

                File.WriteAllBytes(cacheFilePath, cachedFileContent);
            }
            catch
            {
                // Cache is optional.
            }
        }

        private static MemoryStream GetArchiveStream(byte[] fileContent)
        {
            try
            {
                var decompressor = new Decompressor();
                return new MemoryStream(decompressor.Unwrap(fileContent).ToArray());
            }
            catch (ZstdException)
            {
                return new MemoryStream(fileContent);
            }
        }

        private async Task<bool> ConfirmAndExtractAsync(MemoryStream archiveStream, string targetDirName, WorkshopSubmissionItem.SubmissionInstallState originalInstallState)
        {
            try
            {
                using var archive = new ZipArchive(archiveStream);
                if (archive.Entries.Count == 0)
                {
                    await AppDialog.ShowMessageAsync("Submission contains no data! Refusing to install.");
                    RestoreInstallState(originalInstallState);
                    return false;
                }

                var files = archive.Entries
                    .Select(entry => (entry.FullName.StartsWith("sounds/") ? entry.FullName : entry.Name).Trim())
                    .Where(name => name.Length > 0)
                    .Select(name => new WorkshopSubmissionFile(name, true))
                    .OrderBy(file => file)
                    .ToList();

                var installConfirmationWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Install);
                if (await installConfirmationWindow.ShowAsync() != ContentDialogResult.Primary)
                {
                    RestoreInstallState(originalInstallState);
                    return false;
                }

                if (!TryDeleteDirectory(targetDirName, originalInstallState))
                    return false;

                archive.ExtractToDirectory(targetDirName);
                return true;
            }
            catch (Exception exception) when (exception is IOException || exception is InvalidDataException)
            {
                await AppDialog.ShowMessageAsync("Submission contains invalid data! Refusing to install.");
                RestoreInstallState(originalInstallState);
                return false;
            }
        }

        private bool TryDeleteDirectory(string targetDirName, WorkshopSubmissionItem.SubmissionInstallState originalInstallState)
        {
            try
            {
                Directory.Delete(targetDirName, true);
                return true;
            }
            catch (DirectoryNotFoundException)
            {
                return true;
            }
            catch (IOException)
            {
                _ = AppDialog.ShowMessageAsync($"Couldn't access \"{targetDirName}\". Please delete that directory and try again!");
                RestoreInstallState(originalInstallState);
                return false;
            }
        }

        private void WriteMetadata(string targetDirName)
        {
            var metadataJson = new JObject
            {
                ["name"] = m_SubmissionItem.Name,
                ["author"] = m_SubmissionItem.Author,
                ["description"] = m_SubmissionItem.Description,
                ["version"] = m_SubmissionItem.Version,
                ["lastupdated"] = m_SubmissionItem.LastUpdated,
                ["sha256"] = m_SubmissionItem.Sha256
            };

            File.WriteAllText(Path.Combine(targetDirName, "metadata.json"), metadataJson.ToString());
        }

        private void RestoreInstallState(WorkshopSubmissionItem.SubmissionInstallState originalInstallState)
        {
            m_SubmissionItem.InstallState = originalInstallState;
        }

        private static string GetFileSha256(byte[] buffer)
        {
            var builder = new StringBuilder();
            using var hash = SHA256.Create();
            foreach (var b in hash.ComputeHash(buffer))
                builder.Append(b.ToString("x2"));
            return builder.ToString();
        }
    }
}
