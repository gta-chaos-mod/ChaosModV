namespace TwitchChatVotingProxy
{
    enum EVotingMode
    {
        MAJORITY,
        PERCENTAGE,
        ANTIMAJORITY
    }

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
            if (Enum.TryParse<EVotingMode>(str, ignoreCase: true, out var mode))
                return mode;
            return null;
        }
    }
}
