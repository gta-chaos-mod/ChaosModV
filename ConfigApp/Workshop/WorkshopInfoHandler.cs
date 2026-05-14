using System.Windows.Input;
using ConfigApp.Infrastructure;

namespace ConfigApp.Workshop
{
    internal sealed class WorkshopInfoHandler : ICommand
    {
        public event EventHandler? CanExecuteChanged
        {
            add { }
            remove { }
        }

        private readonly WorkshopSubmissionItem m_SubmissionItem;

        public WorkshopInfoHandler(WorkshopSubmissionItem submissionItem)
        {
            m_SubmissionItem = submissionItem;
        }

        public bool CanExecute(object? parameter) => true;

        public async void Execute(object? parameter)
        {
            await ExecuteAsync();
        }

        private Task ExecuteAsync()
        {
            return AppDialog.ShowMessageAsync($@"Name: {m_SubmissionItem.Name}
Author: {m_SubmissionItem.Author}
Version: {m_SubmissionItem.Version}
Id: {m_SubmissionItem.Id}
SHA256: {m_SubmissionItem.Sha256}

Description: {m_SubmissionItem.Description}", "Submission Info");
        }
    }
}
