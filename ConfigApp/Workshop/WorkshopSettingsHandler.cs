using System.IO;
using System.Windows.Input;
using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ConfigApp.Workshop
{
    public class WorkshopSettingsHandler : ICommand
    {
        public event EventHandler? CanExecuteChanged
        {
            add { }
            remove { }
        }

        private readonly WorkshopSubmissionItem m_SubmissionItem;
        private readonly WorkshopSubmissionFileHandler m_FileHandler;

        public WorkshopSettingsHandler(WorkshopSubmissionItem submissionItem, WorkshopSubmissionFileHandler fileHandler)
        {
            m_SubmissionItem = submissionItem;
            m_FileHandler = fileHandler;
        }

        public bool CanExecute(object? parameter)
        {
            return true;
        }

        public async void Execute(object? parameter)
        {
            List<WorkshopSubmissionFile> files;

            try
            {
                m_FileHandler.ReloadFiles();
                files = m_FileHandler.GetSubmissionFiles();
                m_SubmissionItem.UpdateSearchTerms();
            }
            catch (Exception ex)
            {
                await AppDialog.ShowMessageAsync(ex.Message);
                return;
            }

            var editWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Edit, m_FileHandler.SubmissionDirectory, m_SubmissionItem.HighlightedFiles);
            if (await editWindow.ShowAsync() != ContentDialogResult.Primary)
                return;

            try
            {
                m_FileHandler.SetSettings(editWindow.FileStates);
                m_SubmissionItem.UpdateSearchTerms();
            }
            catch (Exception)
            {
                await AppDialog.ShowMessageAsync("Error while saving settings! Check that workshop folder has write permissions");
            }
        }
    }
}
