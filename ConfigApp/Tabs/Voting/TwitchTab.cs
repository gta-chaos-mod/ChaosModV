using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Voting
{
    public class TwitchTab : Tab
    {
        private TextBox m_ChannelName;
        private TextBox m_UserName;

        private PasswordBox m_Token;

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(340f));
            PushNewColumn(new GridLength(10f));
            PushNewColumn(new GridLength(150f));
            PushNewColumn(new GridLength(250f));
            PushNewColumn(new GridLength(10f));
            PushNewColumn(new GridLength());

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
        }

        public override void OnLoadValues()
        {
            m_ChannelName.Text = OptionsManager.TwitchFile.ReadValue("TwitchChannelName");
            m_UserName.Text = OptionsManager.TwitchFile.ReadValue("TwitchUserName");

            m_Token.Password = OptionsManager.TwitchFile.ReadValue("TwitchChannelOAuth");
        }

        public override void OnSaveValues()
        {
            OptionsManager.TwitchFile.WriteValue("TwitchChannelName", m_ChannelName.Text);
            OptionsManager.TwitchFile.WriteValue("TwitchUserName", m_UserName.Text);

            OptionsManager.TwitchFile.WriteValue("TwitchChannelOAuth", m_Token.Password);
        }
    }
}
