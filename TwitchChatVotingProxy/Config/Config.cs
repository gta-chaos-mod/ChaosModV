using Serilog;
using Shared;
using System.Linq;

namespace TwitchChatVotingProxy.Config
{
    class Config : IConfig
    {
        public static readonly string KEY_OVERLAY_SERVER_PORT = "OverlayServerPort";
        public static readonly string KEY_TWITCH_CHANNEL_NAME = "TwitchChannelName";
        public static readonly string KEY_TWITCH_CHANNEL_OAUTH = "TwitchChannelOAuth";
        public static readonly string KEY_TWITCH_CHANNEL_USER_NAME = "TwitchUserName";
        public static readonly string KEY_OVERLAY_MODE = "VotingOverlayMode";
        public static readonly string KEY_RETAIN_INITIAL_VOTES = "VotingChanceSystemRetainChance";
        public static readonly string KEY_VOTING_CHANCE_SYSTEM = "VotingChanceSystem";
        public static readonly string KEY_PERMITTED_USERNAMES = "PermittedUsernames";
        public static readonly string KEY_VOTEABLE_PREFIX = "VoteablePrefix";

        public EOverlayMode? OverlayMode { get; set; }
        public int? OverlayServerPort { get; set; }
        public bool RetainInitalVotes { get; set; }
        public EVotingMode? VotingMode { get; set; }
        public string TwitchChannelName { get; set; }
        public string TwitchOAuth { get; set; }
        public string TwitchUserName { get; set; }
        public string[] PermittedUsernames { get; set; }
        public string VoteablePrefix { get; set; }

        private ILogger logger = Log.Logger.ForContext<Config>();
        private OptionsFile optionsFile;

        public Config(string file, params string[] compatFileNames)
        {
            var files = compatFileNames.Prepend(file);

            if (!File.Exists(file))
            {
                bool foundFile = false;
                foreach (var compatFileName in compatFileNames)
                {
                    if (File.Exists(compatFileName))
                    {
                        file = compatFileName;
                        foundFile = true;
                        break;
                    }
                }

                if (!foundFile)
                {
                    logger.Warning($"Config file \"{file}\" not found");
                    return;
                }
            }

            // If the file does exist, read its content
            optionsFile = new OptionsFile(file);
            optionsFile.ReadFile();

            OverlayServerPort = optionsFile.ReadValueInt(KEY_OVERLAY_SERVER_PORT, -1);
            if (OverlayServerPort == -1) OverlayServerPort = null;
            RetainInitalVotes = optionsFile.ReadValueBool(KEY_RETAIN_INITIAL_VOTES, false, "TwitchVotingChanceSystemRetainChance");
            TwitchChannelName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_NAME);
            TwitchOAuth = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_OAUTH);
            TwitchUserName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_USER_NAME);
            VotingMode = optionsFile.ReadValueInt(KEY_VOTING_CHANCE_SYSTEM, 0, "TwitchVotingChanceSystem") == 0 ? EVotingMode.MAJORITY : EVotingMode.PERCENTAGE;
            OverlayMode = (EOverlayMode)optionsFile.ReadValueInt(KEY_OVERLAY_MODE, 0, "TwitchVotingOverlayMode");

            var tmpPermittedUsernames = optionsFile.ReadValue(KEY_PERMITTED_USERNAMES, "", "TwitchPermittedUsernames").ToLower();  // lower case the username to allow case-insensitive comparisons
            if (tmpPermittedUsernames.Length > 0)
            {
                PermittedUsernames = tmpPermittedUsernames.Split(',');

                for (var i = 0; i < PermittedUsernames.Length; i++)
                {
                    // remove any potential whitespaces around the usernames
                    PermittedUsernames[i] = PermittedUsernames[i].Trim();
                }
            }
            else
            {
                PermittedUsernames = new string[0];
            }

            VoteablePrefix = optionsFile.ReadValue(KEY_VOTEABLE_PREFIX, "");
        }
    }
}
