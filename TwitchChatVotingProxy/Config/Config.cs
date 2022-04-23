using Serilog;
using Shared;
using System.IO;

namespace VotingProxy.Config
{
    class Config : IConfig
    {

        public static readonly string KEY_OBS_OVERLAY_SERVER_PORT = "OBSOverlayServerPort";
        public static readonly string KEY_RETAIN_INITIAL_VOTES = "VotingChanceSystemRetainChance";
        public static readonly string KEY_VOTING_CHANCE_SYSTEM = "VotingChanceSystem";

        public static readonly string KEY_TWITCH_VOTING = "TwitchVoting";
        public static readonly string KEY_TWITCH_CHANNEL_ID = "TwitchChannelID"; 
        public static readonly string KEY_TWITCH_CHANNEL_OAUTH = "TwitchChannelOAuth";
        public static readonly string KEY_TWITCH_CHANNEL_USER_NAME = "TwitchUserName";
        public static readonly string KEY_TWITCH_OVERLAY_MODE = "TwitchVotingOverlayMode";

        public static readonly string KEY_DISCORD_VOTING = "DiscordVoting";
        public static readonly string KEY_DISCORD_GUILD_ID = "DiscordGuildID";
        public static readonly string KEY_DISCORD_CHANNEL_ID = "DiscordChannelID";
        public static readonly string KEY_DISCORD_BOT_OAUTH = "DiscordOAuth"; 

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

                ObsOverlayServerPort = optionsFile.ReadValueInt(KEY_OBS_OVERLAY_SERVER_PORT, -1);
                if (ObsOverlayServerPort == -1) ObsOverlayServerPort = null;
                RetainInitalVotes = optionsFile.ReadValueBool(KEY_RETAIN_INITIAL_VOTES, false);
                VotingMode = optionsFile.ReadValueInt(KEY_VOTING_CHANCE_SYSTEM, 0) == 0 ? EVotingMode.MAJORITY : EVotingMode.PERCENTAGE;

                TwitchChannelId = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_ID);
                TwitchOAuth = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_OAUTH);
                TwitchUserName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_USER_NAME);
                TwitchOverlayMode = (EOverlayMode)optionsFile.ReadValueInt(KEY_TWITCH_OVERLAY_MODE, 0);
                TwitchVoting = optionsFile.ReadValueBool(KEY_TWITCH_VOTING, false);

                DiscordOAuth = optionsFile.ReadValue(KEY_DISCORD_BOT_OAUTH);
                DiscordGuildId = optionsFile.ReadValue(KEY_DISCORD_GUILD_ID);
                DiscordChannelId = optionsFile.ReadValue(KEY_DISCORD_CHANNEL_ID);
                DiscordVoting = optionsFile.ReadValueBool(KEY_DISCORD_VOTING, false);
            }
        }
    }
}
