using Serilog;
using Shared;
using System.IO;

namespace TwitchChatVotingProxy.Config
{
    class Config : IConfig
    {
        public static readonly string KEY_OVERLAY_SERVER_PORT = "OverlayServerPort";
        public static readonly string KEY_TWITCH_CHANNEL_NAME = "TwitchChannelName"; 
        public static readonly string KEY_TWITCH_CHANNEL_OAUTH = "TwitchChannelOAuth";
        public static readonly string KEY_TWITCH_CHANNEL_USER_NAME = "TwitchUserName";
        public static readonly string KEY_TWITCH_VOTING_CHANCE_SYSTEM = "TwitchVotingChanceSystem";
        public static readonly string KEY_TWITCH_OVERLAY_MODE = "TwitchVotingOverlayMode";

        public int? OverlayServerPort { get; set; }
        public string TwitchChannelName { get; set; }
        public string TwitchOAuth { get; set; }
        public string TwitchUserName { get; set; }
        public EVotingMode? VotingMode { get; set; }
        public EOverlayMode? OverlayMode { get; set; }

        private ILogger logger = Log.Logger.ForContext<Config>();
        private OptionsFile optionsFile;
        
        public Config(string file)
        {
            if (!File.Exists(file))
            {
                logger.Warning($"twitch config file \"{file}\" not found");
            } else
            {
                // If the file does exist, read its content
                optionsFile = new OptionsFile(file);
                optionsFile.ReadFile();

                OverlayServerPort = optionsFile.ReadValueInt(KEY_OVERLAY_SERVER_PORT, -1);
                if (OverlayServerPort == -1) OverlayServerPort = null; 
                TwitchChannelName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_NAME);
                TwitchOAuth = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_OAUTH);
                TwitchUserName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_USER_NAME);
                VotingMode = optionsFile.ReadValueInt(KEY_TWITCH_VOTING_CHANCE_SYSTEM, 0) == 0 ? EVotingMode.MAJORITY : EVotingMode.PERCENTAGE;
                OverlayMode = (EOverlayMode)optionsFile.ReadValueInt(KEY_TWITCH_OVERLAY_MODE, 0);
            }
        }
    }
}
