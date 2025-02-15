using System.Windows;
using System.Windows.Input;

namespace ConfigApp.Workshop
{
    public class WorkshopInfoHandler : ICommand
    {
        public event EventHandler? CanExecuteChanged = null;

        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopInfoHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object? parameter)
        {
            return true;
        }

        public void Execute(object? parameter)
        {
            MessageBox.Show($@"Name: {m_SubmissionItem.Name}
Author: {m_SubmissionItem.Author}
Version: {m_SubmissionItem.Version}
Id: {m_SubmissionItem.Id}
SHA256: {m_SubmissionItem.Sha256}

Description: {m_SubmissionItem.Description}", "Submission Info", MessageBoxButton.OK, MessageBoxImage.None);
        }
    }
}
