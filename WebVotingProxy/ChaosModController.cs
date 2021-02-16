using System;
using System.Collections.Generic;
using System.Linq;
using System.Timers;
using Serilog;
using WebVotingProxy.ChaosPipe;
using WebVotingProxy.VotingReceiver;

namespace WebVotingProxy
{
    internal class ChaosModController
    {
        public static readonly int DISPLAY_UPDATE_TICKRATE = 200;

        private ILogger _logger = Log.Logger.ForContext<ChaosModController>();
        private List<IVoteOption> _activeVoteOptions = new List<IVoteOption>();
        private readonly IChaosPipeClient _chaosPipe;
        private readonly MasterClient _masterClient;
        private readonly Timer _displayUpdateTick = new Timer(DISPLAY_UPDATE_TICKRATE);
        private readonly Random _random = new Random();
        private readonly bool _retainInitialVotes;
        private readonly Dictionary<string, int> _userVotedFor = new Dictionary<string, int>();
        private int _voteCounter;
        private bool _voteRunning;

        public ChaosModController(IChaosPipeClient chaosPipe, MasterClient masterClient, bool retainInitialVotes)
        {
            _chaosPipe = chaosPipe;
            _masterClient = masterClient;
            _retainInitialVotes = retainInitialVotes;
            
            // Setup pipe listeners
            _chaosPipe.OnGetCurrentVotes += OnGetCurrentVotes;
            _chaosPipe.OnGetVoteResult += OnGetVoteResult;
            _chaosPipe.OnNewVote += OnNewVote;

            // Setup receiver listeners
            //this.votingReceiver.OnMessage += OnVoteReceiverMessage;
            _masterClient.Client.On("game:vote", response =>
            {
                string socketId = response.GetValue<string>();
                int voteIdx = response.GetValue<int>(1);
                OnVoteReceiverMessage(new OnMessageArgs
                {
                    ClientId = socketId, Idx = voteIdx
                });
            });

            // Setup display update tick
            _displayUpdateTick.Elapsed += DisplayUpdateTick;
            _displayUpdateTick.Enabled = true;
        }

        /// <summary>
        ///     Does the display update tick and is called by a timer
        /// </summary>
        private void DisplayUpdateTick(object sender, ElapsedEventArgs e)
        {
            //overlayServer.UpdateVoting(activeVoteOptions);
        }

        /// <summary>
        ///     Calculate the voting result by counting them, and returning the one
        ///     with the most votes.
        /// </summary>
        private int GetVoteResultByMajority()
        {
            // Find the highest vote count
            var highestVoteCount = _activeVoteOptions.Max(_ => _.Votes);
            // Get all options that have the highest vote count
            var choosenOptions = _activeVoteOptions.FindAll(_ => _.Votes == highestVoteCount);
            // If we only have one choosen option, use that
            var choosenOption = choosenOptions.Count == 1 ? choosenOptions[0] : choosenOptions[_random.Next(0, choosenOptions.Count)];
            return _activeVoteOptions.IndexOf(choosenOption);
        }

        /// <summary>
        ///     Calculate the voting result by assigning them a percentage based on votes,
        ///     and choosing a random option based on that percentage.
        /// </summary>
        private int GetVoteResultByPercentage()
        {
            // Get total votes
            var votes = _activeVoteOptions.Select(_ => _retainInitialVotes ? _.Votes + 1 : _.Votes).ToList();
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

        /// <summary>
        ///     Is called when the chaos mod pipe requests the current votes (callback)
        /// </summary>
        private void OnGetCurrentVotes(object sender, OnGetCurrentVotesArgs args)
        {
            args.CurrentVotes = _activeVoteOptions.Select(_ => _.Votes).ToList();
        }

        /// <summary>
        ///     Is called when the chaos mod wants to know the voting result (callback)
        /// </summary>
        private void OnGetVoteResult(object sender, OnGetVoteResultArgs e)
        {
            // Evaluate what result calculation to use
            e.ChosenOption = GetVoteResultByMajority();
            /*switch (votingMode)
            {
                case EVotingMode.MAJORITY:
                    e.ChosenOption = GetVoteResultByMajority();
                    break;
                case EVotingMode.PERCENTAGE:
                    e.ChosenOption = GetVoteResultByPercentage();
                    break;
            }*/

            // Vote round ended
            _voteRunning = false;
        }

        /// <summary>
        ///     Is called when the chaos mod start a new vote (callback)
        /// </summary>
        private void OnNewVote(object sender, OnNewVoteArgs e)
        {
            _activeVoteOptions = e.VoteOptionNames.ToList().Select((voteOptionName, index) =>
            {
                // We want the options to alternate between matches.
                // If we are on an even round we basically select the index (+1 for non programmers).
                // If we are on an odd round, we add to the index the option count.
                // This gives us a pattern like following:
                // Round 0: [O1, O2, O3, ...]
                // Round 1: [O4, O5, O6, ...]
                var match = _voteCounter % 2 == 0
                    ? (index + 1).ToString()
                    : (index + 1 + _activeVoteOptions.Count).ToString();

                return (IVoteOption) new VoteOption(voteOptionName, new List<string> {match});
            }).ToList();
            _masterClient.Client.EmitAsync("game:new", _masterClient.RoomId, _activeVoteOptions);
            // Clear the old voted for information
            _userVotedFor.Clear();
            // Increase the vote counter
            _voteCounter++;

            // Vote round started now
            _voteRunning = true;
        }

        /// <summary>
        ///     Is called when the voting receiver receives a message
        /// </summary>
        private void OnVoteReceiverMessage(OnMessageArgs e)
        {
            if (!_voteRunning) return;

            var voteOption = _activeVoteOptions[e.Idx];

            // Check if the player has already voted
            if (!_userVotedFor.TryGetValue(e.ClientId, out var previousVote))
            {
                // If they haven't voted, count his vote
                _userVotedFor.Add(e.ClientId, e.Idx);
                voteOption.Votes++;
            }
            else if (previousVote != e.Idx)
            {
                // If the player has already voted, and it's not the same as before,
                // remove the old vote, and add the new one.
                _userVotedFor.Remove(e.ClientId);
                _activeVoteOptions[previousVote].Votes--;

                _userVotedFor.Add(e.ClientId, e.Idx);
                voteOption.Votes++;
            }
        }
    }
}