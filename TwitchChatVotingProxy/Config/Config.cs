﻿using Serilog;
using Shared;

namespace TwitchChatVotingProxy.Config
{
    class Config : IConfig
    {
        public static readonly string KEY_OVERLAY_SERVER_PORT = "OverlayServerPort";
        public static readonly string KEY_TWITCH_CHANNEL_NAME = "TwitchChannelName";
        public static readonly string KEY_TWITCH_CHANNEL_OAUTH = "TwitchChannelOAuth";
        public static readonly string KEY_TWITCH_CHANNEL_USER_NAME = "TwitchUserName";
        public static readonly string KEY_TWITCH_OVERLAY_MODE = "TwitchVotingOverlayMode";
        public static readonly string KEY_TWITCH_RETAIN_INITIAL_VOTES = "TwitchVotingChanceSystemRetainChance";
        public static readonly string KEY_TWITCH_VOTING_CHANCE_SYSTEM = "TwitchVotingChanceSystem";
        public static readonly string KEY_TWITCH_PERMITTED_USERNAMES = "TwitchPermittedUsernames";
        public static readonly string KEY_VOTEABLE_PREFIX = "VoteablePrefix";

        public EOverlayMode? OverlayMode { get; set; }
        public int? OverlayServerPort { get; set; }
        public bool RetainInitalVotes { get; set; }
        public EVotingMode? VotingMode { get; set; }
        public string TwitchChannelName { get; set; }
        public string TwitchOAuth { get; set; }
        public string TwitchUserName { get; set; }
        public string[] PermittedTwitchUsernames { get; set; }
        public string VoteablePrefix { get; set; }

        private ILogger logger = Log.Logger.ForContext<Config>();
        private OptionsFile optionsFile;

        public Config(string file, string compatFile = null)
        {
            if (!File.Exists(file) && compatFile != null)
            {
                file = compatFile;
            }

            if (!File.Exists(file))
            {
                logger.Warning($"twitch config file \"{file}\" not found");
            }
            else
            {
                // If the file does exist, read its content
                optionsFile = new OptionsFile(file);
                optionsFile.ReadFile();

                OverlayServerPort = optionsFile.ReadValueInt(KEY_OVERLAY_SERVER_PORT, -1);
                if (OverlayServerPort == -1) OverlayServerPort = null;
                RetainInitalVotes = optionsFile.ReadValueBool(KEY_TWITCH_RETAIN_INITIAL_VOTES, false);
                TwitchChannelName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_NAME);
                TwitchOAuth = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_OAUTH);
                TwitchUserName = optionsFile.ReadValue(KEY_TWITCH_CHANNEL_USER_NAME);
                VotingMode = optionsFile.ReadValueInt(KEY_TWITCH_VOTING_CHANCE_SYSTEM, 0) == 0 ? EVotingMode.MAJORITY : EVotingMode.PERCENTAGE;
                OverlayMode = (EOverlayMode)optionsFile.ReadValueInt(KEY_TWITCH_OVERLAY_MODE, 0);

                var tmpPermittedUsernames = optionsFile.ReadValue(KEY_TWITCH_PERMITTED_USERNAMES, "").ToLower();  // lower case the username to allow case-insensitive comparisons
                if (tmpPermittedUsernames.Length > 0)
                {
                    PermittedTwitchUsernames = tmpPermittedUsernames.Split(',');

                    for (var i = 0; i < PermittedTwitchUsernames.Length; i++)
                    {
                        // remove any potential whitespaces around the usernames
                        PermittedTwitchUsernames[i] = PermittedTwitchUsernames[i].Trim();
                    }
                }
                else
                {
                    PermittedTwitchUsernames = new string[0];
                }

                VoteablePrefix = optionsFile.ReadValue(KEY_VOTEABLE_PREFIX, "");
            }
        }
    }
}
