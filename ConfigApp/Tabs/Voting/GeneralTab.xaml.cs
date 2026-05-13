using ConfigApp.Tabs;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs.Voting
{
    public sealed partial class GeneralTab : UserControl, ITabLifecycle
    {
        public GeneralTab()
        {
            InitializeComponent();
            Utils.AttachNumericTextBoxBehavior(SecsBeforeVotingTextBox);
            SetPanelsEnabled(false);
        }

        private void SetPanelsEnabled(bool state)
        {
            MainVotingPanel.IsHitTestVisible = state;
            MainVotingPanel.Opacity = state ? 1.0 : 0.55;
            ProportionalVotingPanel.IsHitTestVisible = state;
            ProportionalVotingPanel.Opacity = state ? 1.0 : 0.55;
        }

        public void OnTabSelected()
        {
        }

        public void OnLoadValues()
        {
            EnableVotingCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("EnableVoting", false, "EnableTwitchVoting");
            SetPanelsEnabled(EnableVotingCheckBox.IsChecked.GetValueOrDefault());
            OverlayModeComboBox.SelectedIndex = OptionsManager.VotingFile.ReadValue("VotingOverlayMode", 0, "TwitchVotingOverlayMode");
            EnableRandomEffectCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("RandomEffectVoteableEnable", true, "TwitchRandomEffectVoteableEnable");
            SecsBeforeVotingTextBox.Text = $"{OptionsManager.VotingFile.ReadValue("VotingSecsBeforeVoting", 0, "TwitchVotingSecsBeforeVoting")}";
            PermittedUserNamesTextBox.Text = OptionsManager.VotingFile.ReadValue<string>("PermittedUsernames", null, "TwitchPermittedUsernames");
            VoteablePrefixTextBox.Text = OptionsManager.VotingFile.ReadValue("VoteablePrefix", "");
            EnableProportionalVotingCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("VotingChanceSystem", false, "TwitchVotingChanceSystem");
            EnableProportionalVotingRetainInitialChanceCheckBox.IsChecked = OptionsManager.VotingFile.ReadValue("VotingChanceSystemRetainChance", true, "TwitchVotingChanceSystemRetainChance");
        }

        public void OnSaveValues()
        {
            OptionsManager.VotingFile.WriteValue("EnableVoting", EnableVotingCheckBox.IsChecked);
            OptionsManager.VotingFile.WriteValue("VotingOverlayMode", OverlayModeComboBox.SelectedIndex);
            OptionsManager.VotingFile.WriteValue("RandomEffectVoteableEnable", EnableRandomEffectCheckBox.IsChecked);
            OptionsManager.VotingFile.WriteValueAsInt("VotingSecsBeforeVoting", SecsBeforeVotingTextBox.Text);
            OptionsManager.VotingFile.WriteValue("PermittedUsernames", PermittedUserNamesTextBox.Text);
            OptionsManager.VotingFile.WriteValue("VoteablePrefix", VoteablePrefixTextBox.Text);
            OptionsManager.VotingFile.WriteValue("VotingChanceSystem", EnableProportionalVotingCheckBox.IsChecked);
            OptionsManager.VotingFile.WriteValue("VotingChanceSystemRetainChance", EnableProportionalVotingRetainInitialChanceCheckBox.IsChecked);
        }

        private void OnEnableVotingClick(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
        {
            SetPanelsEnabled(EnableVotingCheckBox.IsChecked.GetValueOrDefault());
        }
    }
}
