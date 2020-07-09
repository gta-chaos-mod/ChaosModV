using Newtonsoft.Json;
using Serilog;
using Serilog.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.OverlayServer;
using TwitchChatVotingProxy.VotingReceiver;

namespace TwitchChatVotingProxy
{
    class ChaosModController
    {
        public static readonly int DISPLAY_UPDATE_TICKRATE = 200;

        private List<IVoteOption> activeVoteOptions = new List<IVoteOption>();
        private IChaosPipeClient chaosPipe;
        private Timer displayUpdateTick = new Timer(DISPLAY_UPDATE_TICKRATE);
        private IOverlayServer overlayServer;
        private Dictionary<string, int> userVotedFor = new Dictionary<string, int>();
        private Random random = new Random();
        private int voteCounter = 0;
        private EVotingMode votingMode = EVotingMode.PERCENTAGE;
        private IVotingReceiver votingReceiver;

        public ChaosModController(
            IChaosPipeClient chaosPipe,
            IOverlayServer overlayServer,
            IVotingReceiver votingReceiver
        ) {
            this.chaosPipe = chaosPipe;
            this.overlayServer = overlayServer;
            this.votingReceiver = votingReceiver;

            // Setup pipe listeners
            this.chaosPipe.OnGetVoteResult += OnGetVoteResult;
            this.chaosPipe.OnNewVote += OnNewVote;

            // Setup receiver listeners
            this.votingReceiver.OnMessage += OnVoteReceiverMessage;

            // Setup display update tick
            displayUpdateTick.Elapsed += DisplayUpdateTick;
            displayUpdateTick.Enabled = true;
        }

        private void DisplayUpdateTick(object sender, ElapsedEventArgs e)
        {
            overlayServer.UpdateVoting(activeVoteOptions);
        }
        private int GetVoteResultByMajority()
        {
            // Find the highest vote count
            var highestVoteCount = activeVoteOptions.Max(_ => _.Votes);
            // Get all options that have the highest vote count
            var choosenOptions = activeVoteOptions.FindAll(_ => _.Votes == highestVoteCount);
            IVoteOption choosenOption;
            // If we only have one choosen option, use that
            if (choosenOptions.Count == 1) choosenOption = choosenOptions[0];
            // Otherwise we have more that one option with the same vote count,
            // and choose one at random
            else choosenOption = choosenOptions[random.Next(0, choosenOptions.Count)];

            return activeVoteOptions.IndexOf(choosenOption);
        }
        private int GetVoteResultByPercentage()
        {
            // Get total votes
            var totalVotes = 0;
            activeVoteOptions.ForEach(_ => totalVotes += _.Votes);
            // If we have no votes, choose one at random
            if (totalVotes == 0) return random.Next(0, activeVoteOptions.Count);
            // Select a random vote from all votes
            var selectedVote = random.Next(1, totalVotes + 1);
            // Now find out in what vote range/option that vote is
            var voteRange = 0;
            var selectedOption = 0;
            for (var i = 0; i < activeVoteOptions.Count; i++)
            {
                voteRange += activeVoteOptions[i].Votes;
                if (selectedVote <= voteRange)
                {
                    selectedOption = i;
                    break;
                }
            }

            // Return the selected vote range/option
            return selectedOption;
        }
        private void OnGetVoteResult(object sender, OnGetVoteResultArgs e)
        {
            // Tell the overlay server that the vote has ended
            overlayServer.EndVoting();

            // Evaluate what result calculation to use
            switch (votingMode)
            {
                case EVotingMode.MAJORITY:
                    e.ChoosenOption = GetVoteResultByMajority();
                    break;
                case EVotingMode.PERCENTAGE:
                    e.ChoosenOption = GetVoteResultByPercentage();
                    break;
            }
        }
        private void OnNewVote(object sender, OnNewVoteArgs e)
        {
            activeVoteOptions = e.VoteOptionNames.ToList().Select((voteOptionName, index) =>
            {
                // We want the options to alternate between matches.
                // If we are on a event round we basically select the index (+1 for none programmers).
                // If we are on a odd round, we add to the index the option count.
                // This gives us a pattern like following:
                // Round 0: [O1, O2, O3, ...]
                // Round 1: [O4, O5, O6, ...]
                var match = voteCounter % 2 == 0
                    ? (index + 1).ToString()
                    : (index + 1 + activeVoteOptions.Count).ToString();
                return (IVoteOption)new VoteOption(voteOptionName, new List<string>() { match });
            }).ToList();
            // Inform the overlay server about a new vote
            overlayServer.NewVoting(activeVoteOptions);
            // Clear the old voted for information
            userVotedFor.Clear();
        }
        private void OnVoteReceiverMessage(object sender, OnMessageArgs e)
        {
            for (int i = 0; i < activeVoteOptions.Count; i++)
            {
                var voteOption = activeVoteOptions[i];

                if (voteOption.Matches.Contains(e.Message))
                {
                    int previousVote;

                    // Check if the player has already voted
                    if (!userVotedFor.TryGetValue(e.ClientId, out previousVote))
                    {
                        // If he hasn't voted, count his vote
                        userVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;
       
                    } else if (previousVote != i)
                    {
                        // If the player has already voted, and it's not the same as before,
                        // remove the old vote, and add the new one.
                        userVotedFor.Remove(e.ClientId);
                        activeVoteOptions[previousVote].Votes--;
                        
                        userVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;
                    }

                    break;
                }
            }
        }
    }
}
