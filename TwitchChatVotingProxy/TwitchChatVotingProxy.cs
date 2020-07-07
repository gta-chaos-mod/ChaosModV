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




namespace TwitchChatVotingProxy
{


    class TwitchChatVotingProxy
    {
        private static ChaosPipeClient chaosPipe;
        private static TwitchVoting twitchVoting;

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
            twitchVoting.CreateVote(voteOptions);
        }

        private static void onGetVoteResult(object sender, GetVoteResultEventArgs e)
        {
            var activeVoteOptions = twitchVoting.ActiveVoteOptions;
            int selectedOption = 0;
            for (var i = 0; i < activeVoteOptions.Count; i++)
            {
                if (activeVoteOptions[i].Votes > activeVoteOptions[selectedOption].Votes) selectedOption = i;
            }

            Log.Logger.Information($"vote result requested, choosen option: {activeVoteOptions[selectedOption].Label}");

            e.SelectedOption = selectedOption;
        }
    }
}