using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Voting
{
    public class TwitchTab : Tab
    {
        private CheckBox? m_EnableTwitchVoting = null;

        private TextBox? m_ChannelName = null;
        private TextBox? m_UserName = null;

        private PasswordBox? m_Token = null;

        private void SetElementsEnabled(bool state)
        {
            if (m_ChannelName is not null)
            {
                m_ChannelName.IsEnabled = state;
            }
            if (m_UserName is not null)
            {
                m_UserName.IsEnabled = state;
            }

            if (m_Token is not null)
            {
                m_Token.IsEnabled = state;
            }
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
            m_EnableTwitchVoting = new CheckBox()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Center,
                Content = "Enable Twitch Voting"
            };
            m_EnableTwitchVoting.Click += (sender, eventArgs) =>
            {
                SetElementsEnabled(m_EnableTwitchVoting.IsChecked.GetValueOrDefault());
            };
            PushRowElement(m_EnableTwitchVoting);
            PopRow();

            PushRowSpacedPair("Channel Name", m_ChannelName = new TextBox()
            {
                Width = 120f,
                Height = 20f
            });
            PushRowSpacedPair("Username", m_UserName = new TextBox()
            {
                Width = 120f,
                Height = 20f
            });
            PopRow();

            PushRowSpacedPair("OAuth Token", m_Token = new PasswordBox()
            {
                Width = 120f,
                Height = 20f
            });

            SetElementsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EnableTwitchVoting is not null)
            {
                m_EnableTwitchVoting.IsChecked = OptionsManager.TwitchFile.ReadValueBool("EnableVotingTwitch", false);
                SetElementsEnabled(m_EnableTwitchVoting.IsChecked.GetValueOrDefault());
            }

            if (m_ChannelName is not null)
            {
                m_ChannelName.Text = OptionsManager.TwitchFile.ReadValue("TwitchChannelName");
            }
            if (m_UserName is not null)
            {
                m_UserName.Text = OptionsManager.TwitchFile.ReadValue("TwitchUserName");
            }

            if (m_Token is not null)
            {
                m_Token.Password = OptionsManager.TwitchFile.ReadValue("TwitchChannelOAuth");
            }
        }

        public override void OnSaveValues()
        {
            OptionsManager.TwitchFile.WriteValue("EnableVotingTwitch", m_EnableTwitchVoting?.IsChecked);

            OptionsManager.TwitchFile.WriteValue("TwitchChannelName", m_ChannelName?.Text);
            OptionsManager.TwitchFile.WriteValue("TwitchUserName", m_UserName?.Text);

            OptionsManager.TwitchFile.WriteValue("TwitchChannelOAuth", m_Token?.Password);
        }
    }
}
