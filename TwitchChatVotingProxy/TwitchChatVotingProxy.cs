using Shared;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Timers;
using TwitchChatVotingProxy.Properties;




namespace TwitchChatVotingProxy
{


    class TwitchChatVotingProxy
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        private static ChaosPipeClient chaosPipe;
        private static TwitchVoting twitchVoting;

        private static void Main(string[] args)
        {

            AllocConsole();
            string[] lines = { };
            try
            {
                lines = System.IO.File.ReadAllLines("chaosmod/twitch2.txt");
            } catch (Exception e)
            {
                Console.Write(e);
            }

            Console.Write("New version");
            twitchVoting = new TwitchVoting(lines[0], lines[1]);
 

            try
            {
                chaosPipe = new ChaosPipeClient();

                chaosPipe.OnNewVote += onNewVote;
                chaosPipe.OnGetVoteResult += onGetVoteResult;
            }
            catch (Exception e)
            {
                Console.Write(e);
            }

            Console.ReadKey();
        }

        private static void onNewVote(object sender, List<IVoteOption> voteOptions)
        {
            twitchVoting.createVote(voteOptions);
        }

        private static void onGetVoteResult(object sender, GetVoteResultEventArgs e)
        {
            var activeVoteOptions = twitchVoting.ActiveVoteOptions;
            int selectedOption = 0;
            for (var i = 0; i < activeVoteOptions.Count; i++)
            {
                if (activeVoteOptions[i].Votes > activeVoteOptions[selectedOption].Votes) selectedOption = i;
            }

            e.SelectedOption = selectedOption;
        }
    }
}