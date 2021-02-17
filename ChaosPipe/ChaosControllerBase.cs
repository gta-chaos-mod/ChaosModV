using System;
using System.Collections.Generic;
using System.Linq;
using Serilog;

namespace ChaosPipe
{
    public abstract class ChaosControllerBase
    {
        protected ILogger Logger;
        protected List<IVoteOption> ActiveVoteOptions;
        protected int VoteCounter;
        protected bool VoteRunning;

        private readonly Random _random;

        protected readonly bool RetainInitialVotes;
        protected readonly Dictionary<string, int> UserVotedFor;

        protected ChaosControllerBase(IChaosPipeClient pipeClient, bool retainInitialVotes)
        {
            _random = new Random();
            RetainInitialVotes = retainInitialVotes;
            UserVotedFor = new Dictionary<string, int>();
            Logger = Log.Logger.ForContext<ChaosControllerBase>();
            ActiveVoteOptions = new List<IVoteOption>();

            pipeClient.OnGetCurrentVotes += OnGetCurrentVotes;
        }

        /// <summary>
        ///     Is called when the chaos mod pipe requests the current votes (callback)
        /// </summary>
        private void OnGetCurrentVotes(object sender, OnGetCurrentVotesArgs args)
        {
            args.CurrentVotes = ActiveVoteOptions.Select(_ => _.Votes).ToList();
        }

        /// <summary>
        ///     Calculate the voting result by counting them, and returning the one
        ///     with the most votes.
        /// </summary>
        protected int GetVoteResultByMajority()
        {
            // Find the highest vote count
            var highestVoteCount = ActiveVoteOptions.Max(_ => _.Votes);
            // Get all options that have the highest vote count
            var choosenOptions = ActiveVoteOptions.FindAll(_ => _.Votes == highestVoteCount);
            // If we only have one choosen option, use that
            var choosenOption = choosenOptions.Count == 1 ? choosenOptions[0] : choosenOptions[_random.Next(0, choosenOptions.Count)];
            return ActiveVoteOptions.IndexOf(choosenOption);
        }

        /// <summary>
        ///     Calculate the voting result by assigning them a percentage based on votes,
        ///     and choosing a random option based on that percentage.
        /// </summary>
        protected int GetVoteResultByPercentage()
        {
            // Get total votes
            var votes = ActiveVoteOptions.Select(_ => RetainInitialVotes ? _.Votes + 1 : _.Votes).ToList();
            var totalVotes = 0;
            votes.ForEach(_ => totalVotes += _);
            // If we have no votes, choose one at random
            if (totalVotes == 0) return _random.Next(0, votes.Count);
            // Select a random vote from all votes
            var selectedVote = _random.Next(1, totalVotes + 1);
            // Now find out in what vote range/option that vote is
            var voteRange = 0;
            var selectedOption = 0;
            for (var i = 0; i < votes.Count; i++)
            {
                voteRange += votes[i];
                if (selectedVote <= voteRange)
                {
                    selectedOption = i;
                    break;
                }
            }

            // Return the selected vote range/option
            return selectedOption;
        }

        protected List<IVoteOption> GenerateVotes(OnNewVoteArgs e)
        {
            return e.VoteOptionNames.ToList().Select((voteOptionName, index) =>
            {
                // We want the options to alternate between matches.
                // If we are on an even round we basically select the index (+1 for non programmers).
                // If we are on an odd round, we add to the index the option count.
                // This gives us a pattern like following:
                // Round 0: [O1, O2, O3, ...]
                // Round 1: [O4, O5, O6, ...]
                var match = VoteCounter % 2 == 0
                    ? (index + 1).ToString()
                    : (index + 1 + ActiveVoteOptions.Count).ToString();

                return (IVoteOption) new VoteOption(voteOptionName, new List<string> {match});
            }).ToList();
        }
    }
}
