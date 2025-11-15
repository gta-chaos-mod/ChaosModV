using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Voting
{
    public class StreamerBotTab : Tab
    {
        private CheckBox? m_EnableStreamerBotVoting = null;
        private TextBox? m_Port = null;

        private void SetElementsEnabled(bool state)
        {
            if (m_Port is not null)
                m_Port.IsEnabled = state;
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(340f));
            PushNewColumn(new GridLength(10f));
            PushNewColumn(new GridLength(150f));
            PushNewColumn(new GridLength(250f));
            PushNewColumn(new GridLength(10f));
            PushNewColumn(new GridLength());

            PushRowEmpty();
            PushRowEmpty();
            PushRowEmpty();
            m_EnableStreamerBotVoting = new CheckBox()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Center,
                Content = "Enable Streamer.bot Voting"
            };
            m_EnableStreamerBotVoting.Click += (sender, eventArgs) =>
            {
                SetElementsEnabled(m_EnableStreamerBotVoting.IsChecked.GetValueOrDefault());
            };
            PushRowElement(m_EnableStreamerBotVoting);
            PopRow();

            PushRowSpacedPair("Port", m_Port = new TextBox()
            {
                Width = 120f,
                Height = 20f
            });

            SetElementsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EnableStreamerBotVoting is not null)
            {
                m_EnableStreamerBotVoting.IsChecked = OptionsManager.VotingFile.ReadValue("EnableVotingStreamerBot", false);
                SetElementsEnabled(m_EnableStreamerBotVoting.IsChecked.GetValueOrDefault());
            }
            if (m_Port is not null)
                m_Port.Text = OptionsManager.VotingFile.ReadValue("StreamerBotPort", 8080).ToString();
        }

        public override void OnSaveValues()
        {
            OptionsManager.VotingFile.WriteValue("EnableVotingStreamerBot", m_EnableStreamerBotVoting?.IsChecked);
            if (int.TryParse(m_Port?.Text, out int port))
            {
                OptionsManager.VotingFile.WriteValue("StreamerBotPort", port);
            }
        }
    }
}
