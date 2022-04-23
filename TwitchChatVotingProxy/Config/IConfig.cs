using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VotingProxy.Config
{
    interface IConfig
    {
        public int? ObsOverlayServerPort { get; set; }
        public bool RetainInitalVotes { get; set; }
        public EVotingMode? VotingMode { get; set; }

        public EOverlayMode? TwitchOverlayMode { get; set; }
        public string TwitchChannelId { get; set; }
        public string TwitchOAuth { get; set; }
        public string TwitchUserName { get; set; }
        public bool TwitchVoting { get; set; }

        public string DiscordOAuth { get; set; }
        public string DiscordGuildId { get; set; }
        public string DiscordChannelId { get; set; }
        public bool DiscordVoting { get; set; }
    }
}
