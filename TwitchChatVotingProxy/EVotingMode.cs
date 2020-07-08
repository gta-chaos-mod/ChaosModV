using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using TwitchLib.Api.Core.Enums;

namespace TwitchChatVotingProxy
{
    enum EVotingMode
    {
        MAJORITY,
        PERCENTAGE,
    }

    static class VotingModeDict
    {
        public static Dictionary<EVotingMode, string> Dict = new Dictionary<EVotingMode, string>()
        {
            { EVotingMode.MAJORITY, "MAJORITY" },
            { EVotingMode.PERCENTAGE, "PERCENTAGE" },
        };

        public static string Lookup(EVotingMode votingMode)
        {
            string str;
            Dict.TryGetValue(votingMode, out str);
            return str;
        }

        public static EVotingMode? ReverseLookup(string str)
        {
            EVotingMode? votingMode = null;

            foreach(var entry in Dict)
            {
                if (entry.Value == str)
                {
                    votingMode = entry.Key;
                    break;
                }
            }

            return votingMode;
        }
    }
}