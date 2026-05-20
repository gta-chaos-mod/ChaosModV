using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Voting
{
    public sealed partial class TwitchTab : UserControl, ITabLifecycle
    {
        public TwitchTab()
        {
            InitializeComponent();
            SetElementsEnabled(false);
        }

        private void SetElementsEnabled(bool state)
        {
            ChannelNameTextBox.IsEnabled = state;
            UserNameTextBox.IsEnabled = state;
            TokenPasswordBox.IsEnabled = state;
        }

        public void OnTabSelected() { }

        public void OnLoadValues()
        {
            EnableTwitchVotingCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("EnableVotingTwitch", false);
            SetElementsEnabled(EnableTwitchVotingCheckBox.IsChecked.GetValueOrDefault());
            ChannelNameTextBox.Text = OptionsManager.VotingFile.ReadValue<string>("TwitchChannelName");
            UserNameTextBox.Text = OptionsManager.VotingFile.ReadValue<string>("TwitchUserName");
            TokenPasswordBox.Password = OptionsManager.VotingFile.ReadValue<string>("TwitchChannelOAuth");
        }

        public void OnSaveValues()
        {
            OptionsManager.VotingFile.WriteValue("EnableVotingTwitch", EnableTwitchVotingCheckBox.IsChecked);
            OptionsManager.VotingFile.WriteValue("TwitchChannelName", ChannelNameTextBox.Text);
            OptionsManager.VotingFile.WriteValue("TwitchUserName", UserNameTextBox.Text);
            OptionsManager.VotingFile.WriteValue("TwitchChannelOAuth", TokenPasswordBox.Password);
        }

        private void OnEnableTwitchVotingClick(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
        {
            SetElementsEnabled(EnableTwitchVotingCheckBox.IsChecked.GetValueOrDefault());
        }
    }
}
