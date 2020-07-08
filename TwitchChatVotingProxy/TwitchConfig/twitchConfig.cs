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

namespace TwitchChatVotingProxy.TwitchConfig
{
    class TwitchConfig : ITwtichConfig
    {
        public static string KEY_CHANNEL = "TwitchChannelName"; 
        public static string KEY_OAUTH = "TwitchChannelOAuth";
        public static string KEY_USER_NAME = "TwitchUserName";
        public static string KEY_VOTING_MODE = "TwitchVotingMode";

        private ILogger logger = Log.Logger.ForContext<TwitchConfig>();
        private OptionsFile optionsFile;
        
        public TwitchConfig(string file)
        {
            if (!File.Exists(file))
            {
                logger.Warning($"twitch config file \"{file}\" not found");
            } else
            {
                // If the file does exist, read its content
                optionsFile = new OptionsFile(file);
                optionsFile.ReadFile();
                ChannelName = optionsFile.ReadValue(KEY_CHANNEL);
                OAuth = optionsFile.ReadValue(KEY_OAUTH);
                UserName = optionsFile.ReadValue(KEY_USER_NAME);
                VotingMode = VotingModeDict.ReverseLookup(optionsFile.ReadValue(KEY_VOTING_MODE, null));
            }
        }

        public string ChannelName { get; } = null;
        public string OAuth { get; } = null;
        public string UserName { get; } = null;
        public EVotingMode? VotingMode { get; }
    }
}
