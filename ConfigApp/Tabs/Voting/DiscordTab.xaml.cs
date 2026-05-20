using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Voting
{
    public sealed partial class DiscordTab : UserControl, ITabLifecycle
    {
        public DiscordTab()
        {
            InitializeComponent();
            Utils.AttachNumericTextBoxBehavior(GuildIdTextBox);
            Utils.AttachNumericTextBoxBehavior(ChannelIdTextBox);
            SetElementsEnabled(false);
        }

        private void SetElementsEnabled(bool state)
        {
            TokenPasswordBox.IsEnabled = state;
            GuildIdTextBox.IsEnabled = state;
            ChannelIdTextBox.IsEnabled = state;
        }

        public void OnTabSelected() { }

        public void OnLoadValues()
        {
            EnableDiscordVotingCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("EnableVotingDiscord", false);
            SetElementsEnabled(EnableDiscordVotingCheckBox.IsChecked.GetValueOrDefault());
            TokenPasswordBox.Password = OptionsManager.VotingFile.ReadValue<string>("DiscordBotToken");
            GuildIdTextBox.Text = OptionsManager.VotingFile.ReadValue<string>("DiscordGuildId");
            ChannelIdTextBox.Text = OptionsManager.VotingFile.ReadValue<string>("DiscordChannelId");
        }

        public void OnSaveValues()
        {
            OptionsManager.VotingFile.WriteValue("EnableVotingDiscord", EnableDiscordVotingCheckBox.IsChecked);
            OptionsManager.VotingFile.WriteValue("DiscordBotToken", TokenPasswordBox.Password);
            OptionsManager.VotingFile.WriteValue("DiscordGuildId", GuildIdTextBox.Text);
            OptionsManager.VotingFile.WriteValue("DiscordChannelId", ChannelIdTextBox.Text);
        }

        private void OnEnableDiscordVotingClick(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
        {
            SetElementsEnabled(EnableDiscordVotingCheckBox.IsChecked.GetValueOrDefault());
        }
    }
}
