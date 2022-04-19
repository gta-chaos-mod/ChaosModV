using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VotingProxy
{
    /// <summary>
    /// Config object for the twitch implementation of the voting receiver
    /// </summary>
    class TwitchVotingReceiverConfig
    {
        public TwitchVotingReceiverConfig(string channelId, string oAuth, string userName, EChannelType channelType = EChannelType.TWITCH)
        {
            // Validate config
            if (channelId == null) throw new Exception("channel name cannot be null");
            if (oAuth == null) throw new Exception("oAuth cannot be null");
            if (userName == null) throw new Exception("user name cannot be null");

            ChannelId = channelId;
            OAuth = oAuth;
            UserName = userName;
            ChannelType = channelType;
        }

        public string ChannelId { get; set; }
        public string OAuth { get; set; }
        public string UserName { get; set; }
        public EChannelType ChannelType { get; set; }
    }

    class DiscordVotingRecieverConfig
    {
        public DiscordVotingRecieverConfig(string channelId, string guildID, string token)
        {
            if (channelId == null) throw new Exception("discord channel id cannot be null");
            if (token == null) throw new Exception("discord bot token cannot be null");

            ChannelId = channelId;
            GuildID = guildID;
            Token = token;
        }
    
        public string Token { get; set; }
        public string ChannelId { get; set; }
        public string GuildID { get; set; }
    }

}
