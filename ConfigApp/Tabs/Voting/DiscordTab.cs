using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Voting
{
    public class DiscordTab : Tab
    {
        private CheckBox? m_EnableDiscordVoting = null;
        private PasswordBox? m_Token = null;
        private TextBox? m_GuildId = null;
        private TextBox? m_ChannelId = null;

        private void SetElementsEnabled(bool state)
        {
            if (m_Token is not null)
                m_Token.IsEnabled = state;
            if (m_GuildId is not null)
                m_GuildId.IsEnabled = state;
            if (m_ChannelId is not null)
                m_ChannelId.IsEnabled = state;
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
            PushRowExpandElement(new TextBlock()
            {
                Text = "NOTE: This is still an experimental feature!",
                HorizontalAlignment = HorizontalAlignment.Left
            });
            PopRow();

            PushRowEmpty();
            PushRowEmpty();
            PushRowEmpty();
            m_EnableDiscordVoting = new CheckBox()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Center,
                Content = "Enable Discord Voting"
            };
            m_EnableDiscordVoting.Click += (sender, eventArgs) =>
            {
                SetElementsEnabled(m_EnableDiscordVoting.IsChecked.GetValueOrDefault());
            };
            PushRowElement(m_EnableDiscordVoting);
            PopRow();

            PushRowSpacedPair("Bot Token", m_Token = new PasswordBox()
            {
                Width = 120f,
                Height = 20f
            });
            PopRow();

            PopRow();

            PushRowEmpty();
            PushRowExpandElement(new TextBlock()
            {
                Text = "The options below are only required if you want bot messages\n" +
                    "(e.g. if you have the \"Chat Messages\" voting overlay mode option set)"
            });
            PopRow();

            PushRowSpacedPair("Server ID", m_GuildId = Utils.GenerateCommonNumericOnlyTextBox(default, 125f, 20f));
            PushRowSpacedPair("Channel ID", m_ChannelId = Utils.GenerateCommonNumericOnlyTextBox(default, 125f, 20f));

            SetElementsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EnableDiscordVoting is not null)
            {
                m_EnableDiscordVoting.IsChecked = OptionsManager.VotingFile.ReadValue("EnableVotingDiscord", false);
                SetElementsEnabled(m_EnableDiscordVoting.IsChecked.GetValueOrDefault());
            }

            if (m_Token is not null)
                m_Token.Password = OptionsManager.VotingFile.ReadValue<string>("DiscordBotToken");
            if (m_GuildId is not null)
                m_GuildId.Text = OptionsManager.VotingFile.ReadValue<string>("DiscordGuildId");
            if (m_ChannelId is not null)
                m_ChannelId.Text = OptionsManager.VotingFile.ReadValue<string>("DiscordChannelId");
        }

        public override void OnSaveValues()
        {
            OptionsManager.VotingFile.WriteValue("EnableVotingDiscord", m_EnableDiscordVoting?.IsChecked);
            OptionsManager.VotingFile.WriteValue("DiscordBotToken", m_Token?.Password);
            OptionsManager.VotingFile.WriteValue("DiscordGuildId", m_GuildId?.Text);
            OptionsManager.VotingFile.WriteValue("DiscordChannelId", m_ChannelId?.Text);
        }
    }
}
