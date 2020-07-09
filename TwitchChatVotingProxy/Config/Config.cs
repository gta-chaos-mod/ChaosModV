using Serilog;
using Serilog.Core;
using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.Config
{
    class Config : IConfig
    {
        public static string KEY_CHANNEL = "TwitchChannelName"; 
        public static string KEY_OAUTH = "TwitchChannelOAuth";
        public static string KEY_USER_NAME = "TwitchUserName";
        public static string KEY_VOTING_MODE = "TwitchVotingMode";

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
                TwitchChannelName = optionsFile.ReadValue(KEY_CHANNEL);
                TwitchOAuth = optionsFile.ReadValue(KEY_OAUTH);
                TwitchUserName = optionsFile.ReadValue(KEY_USER_NAME);
                VotingMode = VotingModeDict.ReverseLookup(optionsFile.ReadValue(KEY_VOTING_MODE, null));
            }
        }

        public string TwitchChannelName { get; } = null;
        public string TwitchOAuth { get; } = null;
        public string TwitchUserName { get; } = null;
        public EVotingMode? VotingMode { get; }
    }
}
