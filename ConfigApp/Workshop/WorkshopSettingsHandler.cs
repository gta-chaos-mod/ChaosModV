using System.Windows.Input;
using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Workshop
{
    internal sealed class WorkshopSettingsHandler : ICommand
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

        public bool CanExecute(object? parameter) => true;

        public async void Execute(object? parameter)
        {
            await ExecuteAsync();
        }

        private async Task ExecuteAsync()
        {
            List<WorkshopSubmissionFile> files;

            try
            {
                m_FileHandler.ReloadFiles();
                m_SubmissionItem.UpdateSearchTerms();
                files = m_FileHandler.GetSubmissionFiles();
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
