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
            return m_SubmissionItem.InstallState != WorkshopSubmissionItem.SubmissionInstallState.Installing
                && m_SubmissionItem.InstallState != WorkshopSubmissionItem.SubmissionInstallState.Removing;
        }

        public async void Execute(object? parameter)
        {
            Directory.CreateDirectory("workshop");

            var targetDirName = $"workshop/{m_SubmissionItem.Id}";

            var origInstallState = m_SubmissionItem.InstallState;

            if (origInstallState == WorkshopSubmissionItem.SubmissionInstallState.Installed)
            {
                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Removing;

                if (!await AppDialog.ShowYesNoAsync("Are you sure you want to remove this submission?"))
                {
                    m_SubmissionItem.InstallState = origInstallState;
                    return;
                }

                try
                {
                    Directory.Delete(targetDirName, true);
                    File.Delete($"{targetDirName}.json");
                }
                catch (DirectoryNotFoundException)
                {

                }
                catch (FileNotFoundException)
                {

                }
                catch (IOException)
                {
                    await AppDialog.ShowMessageAsync($"Couldn't access \"{targetDirName}\". Please delete that directory and try again!");
                    m_SubmissionItem.InstallState = origInstallState;
                    return;
                }

                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.NotInstalled;

                Console.Beep();

                return;
            }

            void fatalCleanup()
            {
                m_SubmissionItem.InstallState = origInstallState;
            }

            if (m_SubmissionItem.Id is null || !m_SubmissionItem.Id.All((c) => char.IsLetterOrDigit(c) && (char.IsNumber(c) || char.IsLower(c))))
            {
                await AppDialog.ShowMessageAsync("Invalid submission id! Refusing to install.");
                fatalCleanup();
                return;
            }

            m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installing;

            var targetCacheDirName = $"workshopcache/{m_SubmissionItem.Id}.zip.zst";

            HttpClient httpClient = new();
            try
            {
                string getFileSha256(byte[] buffer)
                {
                    var sha256StrBuilder = new StringBuilder();
                    using (var hash = SHA256.Create())
                    {
                        var resultHash = hash.ComputeHash(buffer);
                        foreach (var b in resultHash)
                            sha256StrBuilder.Append(b.ToString("x2"));
                    }

                    return sha256StrBuilder.ToString();
                }

                byte[]? fileContent = null;
                bool isFileCached = false, isFileCompressed = false;
                if (File.Exists(targetCacheDirName))
                {
                    fileContent = File.ReadAllBytes(targetCacheDirName);
                    var cachedFileHash = getFileSha256(fileContent);

                    if (cachedFileHash == m_SubmissionItem.Sha256)
                    {
                        isFileCached = true;
                        // Cached files are always compressed
                        isFileCompressed = true;
                    }
                }

                if (!isFileCached)
                {
                    var domain = OptionsManager.WorkshopFile.ReadValue("WorkshopCustomUrl", Info.WORKSHOP_DEFAULT_URL);

                    var result = await httpClient.GetAsync($"{domain}/workshop/fetch_submission_data?submission_id={m_SubmissionItem.Id}");
                    if (!result.IsSuccessStatusCode)
                    {
                        await AppDialog.ShowMessageAsync("Error while fetching submission. Please try again!");
                        fatalCleanup();
                        return;
                    }

                    fileContent = await result.Content.ReadAsByteArrayAsync();
                    var fileHash = getFileSha256(fileContent);
                    if (fileHash != m_SubmissionItem.Sha256)
                    {
                        await AppDialog.ShowMessageAsync("SHA256 mismatch! Please refresh submissions and try again!");
                        fatalCleanup();
                        return;
                    }

                    isFileCompressed = result.Headers.Contains("compressed") && result.Headers.GetValues("Compressed").Contains("yes");
                }

                if (fileContent == null)
                {
                    await AppDialog.ShowMessageAsync("File is invalid. Refusing to install!");
                    fatalCleanup();
                    return;
                }

                try
                {
                    Directory.CreateDirectory("workshopcache");

                    // Recompress if necessary for caching
                    var cachedFileContent = fileContent;
                    if (!isFileCompressed)
                    {
                        var compressor = new Compressor(10);
                        cachedFileContent = compressor.Wrap(fileContent).ToArray();
                    }

                    File.WriteAllBytes(targetCacheDirName, cachedFileContent);
                }
                catch (Exception)
                {
                    // Doesn't matter if this fails, it's just a cached file anyways
                }

                var fileStream = new MemoryStream(fileContent);
                if (isFileCompressed)
                    try
                    {
                        var decompressor = new Decompressor();
                        var decompressed = decompressor.Unwrap(fileContent).ToArray();
                        fileStream = new MemoryStream(decompressed.ToArray());
                    }
                    catch (ZstdException)
                    {
                        // File content is not (zstd) compressed even though compressed = yes?
                        // Skip decompression
                    }

                try
                {
                    using ZipArchive archive = new(fileStream);
                    if (archive.Entries.Count == 0)
                    {
                        await AppDialog.ShowMessageAsync("Submission contains no data! Refusing to install.");
                        fatalCleanup();
                        return;
                    }

                    var files = new List<WorkshopSubmissionFile>();
                    foreach (var entry in archive.Entries)
                    {
                        var trimmedName = (entry.FullName.StartsWith("sounds/") ? entry.FullName : entry.Name).Trim();
                        if (trimmedName.Length > 0)
                            files.Add(new WorkshopSubmissionFile(trimmedName, true));
                    }
                    files.Sort();

                    var installConfirmationWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Install);
                    if (await installConfirmationWindow.ShowAsync() != ContentDialogResult.Primary)
                    {
                        fatalCleanup();
                        return;
                    }

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
                        await AppDialog.ShowMessageAsync($"Couldn't access \"{targetDirName}\". Please delete that directory and try again!");
                        fatalCleanup();
                        return;
                    }

                    archive.ExtractToDirectory(targetDirName);
                }
                catch (Exception exception) when (exception is IOException || exception is InvalidDataException)
                {
                    await AppDialog.ShowMessageAsync("Submission contains invalid data! Refusing to install.");
                    fatalCleanup();
                    return;
                }

                var metadataJson = new JObject
                {
                    ["name"] = m_SubmissionItem.Name,
                    ["author"] = m_SubmissionItem.Author,
                    ["description"] = m_SubmissionItem.Description,
                    ["version"] = m_SubmissionItem.Version,
                    ["lastupdated"] = m_SubmissionItem.LastUpdated,
                    ["sha256"] = m_SubmissionItem.Sha256
                };
                File.WriteAllText($"{targetDirName}/metadata.json", metadataJson.ToString());

                m_SubmissionItem.InstallState = WorkshopSubmissionItem.SubmissionInstallState.Installed;

                Console.Beep();
            }
            catch (HttpRequestException)
            {
                await AppDialog.ShowMessageAsync("Error while fetching submission. Submission might have been removed by remote. Please refresh and try again!");
                fatalCleanup();
                return;
            }
        }
    }
}
