using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Voting
{
    public class GeneralTab : Tab
    {
        private readonly List<Grid> m_Grids = new();

        private CheckBox? m_EnableVoting = null;

        private ComboBox? m_OverlayMode = null;
        private CheckBox? m_EnableRandomEffect = null;

        private TextBox? m_SecsBeforeVoting = null;
        private TextBox? m_PermittedUserNames = null;

        private TextBox? m_VoteablePrefix = null;

        private CheckBox? m_EnableProportionalVoting = null;
        private CheckBox? m_EnableProportionalVotingRetainInitialChance = null;

        private static void SetupSettingsGrid(ChaosGrid grid)
        {
            grid.PushNewColumn(new GridLength(340f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength(150f));
            grid.PushNewColumn(new GridLength(250f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength());
        }

        private void SetGridsEnabled(bool state)
        {
            foreach (var grid in m_Grids)
            {
                grid.IsEnabled = state;
            }
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));

            SetRowHeight(new GridLength(120f));

            var headerGrid = new ChaosGrid();
            headerGrid.PushNewColumn(new GridLength(1f, GridUnitType.Star));

            headerGrid.SetRowHeight(new GridLength(80f));
            headerGrid.PushRowElement(new TextBlock()
            {
                Text = "This feature allows viewers to vote for an effect from a collection of random ones each time the timer" +
                " runs out by making use of respective platform's chat feature.\nThere are specific options you can / must configure below." +
                "\nConsult the \"chaosmod/README/voting/voting.txt\" file for more information.",
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            });
            headerGrid.PopRow();

            m_EnableVoting = new CheckBox()
            {
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center,
                Content = "Enable Voting"
            };
            m_EnableVoting.Click += (sender, eventArgs) =>
            {
                SetGridsEnabled(m_EnableVoting.IsChecked.GetValueOrDefault());
            };
            headerGrid.PushRowElement(m_EnableVoting);

            headerGrid.Grid.HorizontalAlignment = HorizontalAlignment.Stretch;

            PushRowElement(headerGrid.Grid);
            PopRow();

            SetRowHeight(new GridLength(120f));

            var bodyGrid = new ChaosGrid();
            SetupSettingsGrid(bodyGrid);

            bodyGrid.PushRowSpacedPair("Voting Overlay Mode", m_OverlayMode = new ComboBox()
            {
                ItemsSource = new string[]
                {
                    "Chat Messages",
                    "In-Game Overlay",
                    "OBS Overlay"
                }
            });
            bodyGrid.PushRowSpacedPair("Enable \"Random Effect\" voteable option", m_EnableRandomEffect = Utils.GenerateCommonCheckBox());
            bodyGrid.PopRow();

            bodyGrid.PushRowSpacedPair("Only enable chat voting X seconds before new effect triggers\n" +
                "(Keep at 0 to allow chat to immediately vote, value can't be 1)", m_SecsBeforeVoting = Utils.GenerateCommonNumericOnlyTextBox());
            bodyGrid.PushRowSpacedPair("Limit voting to usernames (separated by , )", m_PermittedUserNames = new TextBox()
            {
                Width = 200f,
                Height = 20f
            });
            bodyGrid.PopRow();

            bodyGrid.PushRowSpacedPair("Use character as prefix for voteable options", m_VoteablePrefix = new TextBox()
            {
                Width = 40f,
                Height = 20f,
                MaxLength = 1
            });

            m_Grids.Add(bodyGrid.Grid);

            PushRowElement(bodyGrid.Grid);
            PopRow();

            PushRowElement(new TextBlock()
            {
                Text = "---------- Proportional Voting Mode ----------",
                HorizontalAlignment = HorizontalAlignment.Center
            });
            PopRow();

            var footerGrid = new ChaosGrid();
            SetupSettingsGrid(footerGrid);

            footerGrid.SetRowHeight(new GridLength(20f));
            footerGrid.PushRowSpacedPair("Enable proportional voting mode", m_EnableProportionalVoting = Utils.GenerateCommonCheckBox());
            footerGrid.PushRowSpacedPair("Include initial chance in evaluation", m_EnableProportionalVotingRetainInitialChance = Utils.GenerateCommonCheckBox());

            m_Grids.Add(footerGrid.Grid);

            PushRowElement(footerGrid.Grid);

            SetGridsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EnableVoting is not null)
            {
                m_EnableVoting.IsChecked = OptionsManager.TwitchFile.ReadValueBool("EnableVoting", false, "EnableTwitchVoting");
                SetGridsEnabled(m_EnableVoting.IsChecked.GetValueOrDefault());
            }

            if (m_OverlayMode is not null)
            {
                m_OverlayMode.SelectedIndex = OptionsManager.TwitchFile.ReadValueInt("VotingOverlayMode", 0, "TwitchVotingOverlayMode");
            }
            if (m_EnableRandomEffect is not null)
            {
                m_EnableRandomEffect.IsChecked = OptionsManager.TwitchFile.ReadValueBool("RandomEffectVoteableEnable", true, "TwitchRandomEffectVoteableEnable");
            }

            if (m_SecsBeforeVoting is not null)
            {
                m_SecsBeforeVoting.Text = OptionsManager.TwitchFile.ReadValue("VotingSecsBeforeVoting", "0", "TwitchVotingSecsBeforeVoting");
            }
            if (m_PermittedUserNames is not null)
            {
                m_PermittedUserNames.Text = OptionsManager.TwitchFile.ReadValue("PermittedUsernames", null, "TwitchPermittedUsernames");
            }

            if (m_VoteablePrefix is not null)
            {
                m_VoteablePrefix.Text = OptionsManager.TwitchFile.ReadValue("VoteablePrefix", "");
            }

            if (m_EnableProportionalVoting is not null)
            {
                m_EnableProportionalVoting.IsChecked = OptionsManager.TwitchFile.ReadValueBool("VotingChanceSystem", false, "TwitchVotingChanceSystem");
            }
            if (m_EnableProportionalVotingRetainInitialChance is not null)
            {
                m_EnableProportionalVotingRetainInitialChance.IsChecked = OptionsManager.TwitchFile.ReadValueBool("VotingChanceSystemRetainChance", true,
                    "TwitchVotingChanceSystemRetainChance");
            }
        }

        public override void OnSaveValues()
        {
            OptionsManager.TwitchFile.WriteValue("EnableVoting", m_EnableVoting?.IsChecked);

            OptionsManager.TwitchFile.WriteValue("VotingOverlayMode", m_OverlayMode?.SelectedIndex);
            OptionsManager.TwitchFile.WriteValue("RandomEffectVoteableEnable", m_EnableRandomEffect?.IsChecked);

            OptionsManager.TwitchFile.WriteValue("VotingSecsBeforeVoting", m_SecsBeforeVoting?.Text);
            OptionsManager.TwitchFile.WriteValue("PermittedUsernames", m_PermittedUserNames?.Text);

            OptionsManager.TwitchFile.WriteValue("VoteablePrefix", m_VoteablePrefix?.Text);

            OptionsManager.TwitchFile.WriteValue("VotingChanceSystem", m_EnableProportionalVoting?.IsChecked);
            OptionsManager.TwitchFile.WriteValue("VotingChanceSystemRetainChance", m_EnableProportionalVotingRetainInitialChance?.IsChecked);
        }
    }
}
