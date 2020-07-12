using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy
{
    /// <summary>
    /// Config object for the twitch implementation of the voting receiver
    /// </summary>
    class TwitchVotingReceiverConfig
    {
        public TwitchVotingReceiverConfig(string channelName, string oAuth, string userName)
        {
            // Validate config
            if (channelName == null) throw new Exception("twitch cannel name cannot be null");
            if (oAuth == null) throw new Exception("twitch oAuth cannot be null");
            if (userName == null) throw new Exception("twitch user name cannot be null");

            ChannelName = channelName;
            OAuth = oAuth;
            UserName = userName;
        }

        public string ChannelName { get; set; }
        public string OAuth { get; set; }
        public string UserName { get; set; }
    }
}
