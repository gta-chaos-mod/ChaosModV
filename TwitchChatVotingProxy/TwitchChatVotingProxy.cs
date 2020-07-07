using Serilog;
using Serilog.Core;
using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Timers;
using TwitchChatVotingProxy.Properties;


/**
 * TODO: Choose random option when multiple options have the same votes
 */

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        private static ChaosPipeClient chaosPipe;
        private static TwitchVoting twitchVoting;
        private static VotingMode VotingMode = VotingMode.PERCENTAGE;
        private static Random random = new Random();

        private static void Main(string[] args)
        {

            Log.Logger = new LoggerConfiguration()
               .MinimumLevel.Information()
               .WriteTo.File("./chaosmod/chaosProxy.log")
               .CreateLogger();

            Log.Logger.Information("========================================");
            Log.Logger.Information("Starting chaos mod twtich proxy");
            Log.Logger.Information("========================================");

             // Read values from the options file
            try
            {
                OptionsFile votingOptions = new OptionsFile("./chaosmod/twitch.ini");
                votingOptions.ReadFile();
                var channelName = votingOptions.ReadValue("TwitchChannelName", null);
                var oAuth = votingOptions.ReadValue("TwitchChannelOAuth", null);

                // Validate values from the options file
                if (channelName == null)
                {
                    Log.Logger.Fatal("twitch channel name is null, aborting");
                    return;
                }
                if (oAuth == null)
                {
                    Log.Logger.Fatal("twitch oAuth is null, aborting");
                    return;
                }

                twitchVoting = new TwitchVoting(channelName, oAuth);
            } catch (Exception e)
            {
                Log.Logger.Fatal(e, "failed to initialize twitch client, aborting");
            }
     

            // Setup pipe to chaos mod client
            chaosPipe = new ChaosPipeClient();
            chaosPipe.OnNewVote += onNewVote;
            chaosPipe.OnGetVoteResult += onGetVoteResult;

            while (chaosPipe.Open) { };
            Log.Logger.Information("pipe closed, ending programm");
        }

        private static void onNewVote(object sender, List<IVoteOption> voteOptions)
        {
            string options = String.Join(", ", voteOptions.ConvertAll(_ => _.Label).ToArray());
            Log.Logger.Information($"starting new vote, options: {options}");

            Log.Logger.Warning("Remove this -- on new vote");
            voteOptions.ForEach(_ => _.Votes = 1);

            twitchVoting.CreateVote(voteOptions);
        }

        private static void onGetVoteResult(object sender, GetVoteResultEventArgs e)
        {
            var activeVoteOptions = twitchVoting.ActiveVoteOptions;
            // TODO make this dynamic
            int selectedOption = getVoteResultPercentage(activeVoteOptions);
            Log.Logger.Information($"vote result requested, choosen option: {activeVoteOptions[selectedOption].Label}");

            e.SelectedOption = selectedOption;
        }

        private static int getVoteResultMajority()
        {
            var activeVoteOptions = twitchVoting.ActiveVoteOptions;
            int selectedOption = 0;
            for (var i = 0; i < activeVoteOptions.Count; i++)
            {
                if (activeVoteOptions[i].Votes > activeVoteOptions[selectedOption].Votes) selectedOption = i;
            }

            return selectedOption;
        }

        public static int getVoteResultPercentage(List<IVoteOption> voteOptions)
        {
            // Get total votes
            var totalVotes = 0;
            voteOptions.ForEach(_ => totalVotes += _.Votes);

            // If we have no votes, choose one at random
            if (totalVotes == 0) return random.Next(0, voteOptions.Count);

            // Select a random vote from all votes
            var selectedVote = random.Next(0, totalVotes + 1);
            
            // Now find out in what vote range/option that vote is
            var voteRange = 0;
            var selectedOption = 0;
            for (var i = 0; i < voteOptions.Count; i++)
            {
                voteRange += voteOptions[i].Votes;
                if (selectedVote <= voteRange)
                {
                    selectedOption = i;
                    break;
                }
            }

            // Return the selected vote range/option
            return selectedOption;
        }
    }
}