using Serilog;
using Shared;
using System.IO;

namespace VotingProxy.Config
{
    class Config : IConfig
    {
        public static readonly string KEY_OVERLAY_SERVER_PORT = "OverlayServerPort";
        public static readonly string KEY_CHANNEL_TYPE = "ChannelType";
        public static readonly string KEY_CHANNEL_ID = "ChannelID"; 
        public static readonly string KEY_CHANNEL_OAUTH = "ChannelOAuth";
        public static readonly string KEY_CHANNEL_USER_NAME = "UserName";
        public static readonly string KEY_OVERLAY_MODE = "VotingOverlayMode";
        public static readonly string KEY_RETAIN_INITIAL_VOTES = "ChanceSystemRetainChance";
        public static readonly string KEY_VOTING_CHANCE_SYSTEM = "VotingChanceSystem";

        public EOverlayMode? OverlayMode { get; set; }
        public int? OverlayServerPort { get; set; }
        public bool RetainInitalVotes { get; set; }
        public EVotingMode? VotingMode { get; set; }
        public string ChannelId { get; set; }
        public string OAuth { get; set; }
        public string UserName { get; set; }
        public EChannelType ChannelType { get; set; }

        private ILogger logger = Log.Logger.ForContext<Config>();
        private OptionsFile optionsFile;
        
        public Config(string file)
        {
            if (!File.Exists(file))
            {
                logger.Warning($"config file \"{file}\" not found");
            } else
            {
                // If the file does exist, read its content
                optionsFile = new OptionsFile(file);
                optionsFile.ReadFile();

                OverlayServerPort = optionsFile.ReadValueInt(KEY_OVERLAY_SERVER_PORT, -1);
                if (OverlayServerPort == -1) OverlayServerPort = null;
                RetainInitalVotes = optionsFile.ReadValueBool(KEY_RETAIN_INITIAL_VOTES, false);
                ChannelId = optionsFile.ReadValue(KEY_CHANNEL_ID);
                OAuth = optionsFile.ReadValue(KEY_CHANNEL_OAUTH);
                UserName = optionsFile.ReadValue(KEY_CHANNEL_USER_NAME);
                VotingMode = optionsFile.ReadValueInt(KEY_VOTING_CHANCE_SYSTEM, 0) == 0 ? EVotingMode.MAJORITY : EVotingMode.PERCENTAGE;
                OverlayMode = (EOverlayMode)optionsFile.ReadValueInt(KEY_OVERLAY_MODE, 0);
                ChannelType = (EChannelType)optionsFile.ReadValueInt(KEY_CHANNEL_TYPE, 0);
            }
        }
    }
}
