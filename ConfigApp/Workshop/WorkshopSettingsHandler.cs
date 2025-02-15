using System.IO;
using System.Windows;
using System.Windows.Input;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ConfigApp.Workshop
{
    public class WorkshopSettingsHandler : ICommand
    {
        public event EventHandler? CanExecuteChanged = null;

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

        public void Execute(object? parameter)
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
                MessageBox.Show(ex.Message, "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            var editWindow = new WorkshopEditDialog(files, WorkshopEditDialogMode.Edit, m_FileHandler.SubmissionDirectory, m_SubmissionItem.HighlightedFiles);
            editWindow.ShowDialog();

            try
            {
                m_FileHandler.SetSettings(editWindow.FileStates);
                m_SubmissionItem.UpdateSearchTerms();
            }
            catch (Exception)
            {
                MessageBox.Show("Error while saving settings! Check that workshop folder has write permissions", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}
