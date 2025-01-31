namespace TwitchChatVotingProxy
{
    enum EVotingMode
    {
        MAJORITY,
        PERCENTAGE,
        ANTIMAJORITY
    }

    // TODO: instead of doing this (literal) fuckery, we could just use Enum.ToString() and parse
    // the number back to an enum in typescript.
    static class VotingMode
    {
        private static readonly Dictionary<EVotingMode, string> Dict = new()
        {
            { EVotingMode.MAJORITY, "MAJORITY" },
            { EVotingMode.PERCENTAGE, "PERCENTAGE" },
            { EVotingMode.ANTIMAJORITY, "ANTIMAJORITY" },
        };

        /// <summary>
        /// Lookup a voting mode
        /// </summary>
        /// <param name="votingMode">Desired voting option</param>
        /// <returns>String representation of the voting option</returns>
        public static string? Lookup(EVotingMode votingMode)
        {
            Dict.TryGetValue(votingMode, out string? str);
            return str;
        }

        /// <summary>
        /// Lookup a voting mode string
        /// </summary>
        /// <param name="str">String representation of the desired voting option</param>
        /// <returns>Voting option or null if the string could not be found</returns>
        public static EVotingMode? ReverseLookup(string str)
        {
            EVotingMode? votingMode = null;

            foreach (var entry in Dict)
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
