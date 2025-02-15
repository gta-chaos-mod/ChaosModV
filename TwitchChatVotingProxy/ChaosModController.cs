using System;
using System.Timers;
using Serilog;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.OverlayServer;
using TwitchChatVotingProxy.VotingReceiver;
using Timer = System.Timers.Timer;

namespace TwitchChatVotingProxy
{
    class ChaosModController
    {
        public static readonly int DISPLAY_UPDATE_TICKRATE = 200;

        private readonly IChaosPipeClient m_ChaosPipe;
        private readonly IOverlayServer? m_OverlayServer;
        private readonly IVotingReceiver[] m_VotingReceivers;
        private readonly ChaosModControllerConfig m_Config;

        private List<IVoteOption> m_ActiveVoteOptions = new();
        private readonly Timer m_DisplayUpdateTick = new(DISPLAY_UPDATE_TICKRATE);
        private readonly Dictionary<string, int> m_UserVotedFor = new();
        private readonly Random m_Random = new();
        private int m_VoteCounter = 0;
        private bool m_VoteRunning = false;

        public ChaosModController(IChaosPipeClient chaosPipe, IOverlayServer? overlayServer, IVotingReceiver[] votingReceivers,
            ChaosModControllerConfig config)
        {
            m_ChaosPipe = chaosPipe;
            m_OverlayServer = overlayServer;
            m_VotingReceivers = votingReceivers;
            m_Config = config;

            // Setup pipe listeners
            m_ChaosPipe.OnGetCurrentVotes += OnGetCurrentVotes;
            m_ChaosPipe.OnGetVoteResult += OnGetVoteResult;
            m_ChaosPipe.OnNewVote += OnNewVote;
            m_ChaosPipe.OnNoVotingRound += OnNoVotingRound;
            m_ChaosPipe.OnSetVotingMode += OnSetVotingMode;

            // Setup receiver listeners
            foreach (var votingReceiver in m_VotingReceivers)
                votingReceiver.OnMessage += OnVoteReceiverMessage;

            // Setup display update tick
            m_DisplayUpdateTick.Elapsed += DisplayUpdateTick;
            m_DisplayUpdateTick.Enabled = true;
        }

        /// <summary>
        /// Does the display update tick and is called by a timer
        /// </summary>
        private void DisplayUpdateTick(object? sender, ElapsedEventArgs e)
        {
            m_OverlayServer?.UpdateVoting(m_ActiveVoteOptions);
        }
        /// <summary>
        /// Calculate the voting result by counting them, and returning the one
        /// with the most votes.
        /// </summary>
        private int GetVoteResultByMajority()
        {
            // Find the highest vote count
            var highestVoteCount = m_ActiveVoteOptions.Max(_ => _.Votes);
            // Get all options that have the highest vote count
            var choosenOptions = m_ActiveVoteOptions.FindAll(_ => _.Votes == highestVoteCount);
            IVoteOption choosenOption;
            // If we only have one choosen option, use that
            if (choosenOptions.Count == 1) choosenOption = choosenOptions[0];
            // Otherwise we have more than one option with the same vote count
            // and choose one at random
            else choosenOption = choosenOptions[m_Random.Next(0, choosenOptions.Count)];

            return m_ActiveVoteOptions.IndexOf(choosenOption);
        }
        /// <summary>
        /// Calculate the voting result by assigning them a percentage based on votes,
        /// and choosing a random option based on that percentage.
        /// </summary>
        private int GetVoteResultByPercentage()
        {
            // Get total votes
            var votes = m_ActiveVoteOptions.Select(_ => m_Config.RetainInitialVotes ? _.Votes + 1 : _.Votes).ToList();
            var totalVotes = 0;
            votes.ForEach(_ => totalVotes += _);
            // If we have no votes, choose one at random
            if (totalVotes == 0)
                return m_Random.Next(0, votes.Count);
            // Select a random vote from all votes
            var selectedVote = m_Random.Next(1, totalVotes + 1);
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
        /// Calculate the voting result by the option with the lowest votes
        /// </summary>
        private int GetVoteResultByAntiMajority()
        {
            var lowestVoteCount = m_ActiveVoteOptions.Min(_ => _.Votes);
            var chosenOptions = m_ActiveVoteOptions.FindAll(_ => _.Votes == lowestVoteCount);
            IVoteOption chosenOption;
            if (chosenOptions.Count == 1)
            {
                chosenOption = chosenOptions[0];
            }
            else
            {
                chosenOption = chosenOptions[m_Random.Next(0, chosenOptions.Count)];
            }
            return m_ActiveVoteOptions.IndexOf(chosenOption);
        }
        public void OnSetVotingMode(object? sender, OnSetVotingModeArgs onSetVotingModeArgs)
        {
            m_Config.VotingMode = onSetVotingModeArgs.VotingMode;
        }
        /// <summary>
        /// Is called when the chaos mod pipe requests the current votes (callback)
        /// </summary>
        private void OnGetCurrentVotes(object? sender, OnGetCurrentVotesArgs args)
        {
            args.CurrentVotes = m_ActiveVoteOptions.Select(_ => _.Votes).ToList();
        }
        /// <summary>
        /// Is called when the chaos mod wants to know the voting result (callback)
        /// </summary>
        private void OnGetVoteResult(object? sender, OnGetVoteResultArgs e)
        {
            // Tell the overlay server that the vote has ended
            try
            {
                m_OverlayServer?.EndVoting();
            }
            catch (Exception err)
            {
                Log.Error(err, "error occured");
            }

            // Evaluate what result calculation to use
            switch (m_Config.VotingMode)
            {
            case EVotingMode.MAJORITY:
                e.ChosenOption = GetVoteResultByMajority();
                break;
            case EVotingMode.PERCENTAGE:
                e.ChosenOption = GetVoteResultByPercentage();
                break;
            case EVotingMode.ANTIMAJORITY:
                e.ChosenOption = GetVoteResultByAntiMajority();
                break;
            }

            // Vote round ended
            m_VoteRunning = false;
        }
        /// <summary>
        /// Is called when the chaos mod start a new vote (callback)
        /// </summary>
        private async void OnNewVote(object? sender, OnNewVoteArgs e)
        {
            m_ActiveVoteOptions = e.VoteOptionNames.ToList().Select((voteOptionName, index) =>
            {
                // We want the options to alternate between matches.
                // If we are on an even round we basically select the index (+1 for non programmers).
                // If we are on an odd round, we add to the index the option count.
                // This gives us a pattern like following:
                // Round 0: [O1, O2, O3, ...]
                // Round 1: [O4, O5, O6, ...]
                var match = m_VoteCounter % 2 == 0
                    ? m_Config.VoteablePrefix + (index + 1)
                    : m_Config.VoteablePrefix + (index + 1 + m_ActiveVoteOptions.Count);

                return (IVoteOption)new VoteOption(voteOptionName, new List<string>() { match });
            }).ToList();
            // Depending on the overlay mode either inform the overlay server about the new vote or send a chat message aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
            switch (m_Config.OverlayMode)
            {
            case EOverlayMode.CHAT_MESSAGES:
                string msg = "Time for a new effect! Vote between:";
                foreach (IVoteOption voteOption in m_ActiveVoteOptions)
                {
                    msg += "\n";

                    bool firstIndex = true;
                    foreach (string match in voteOption.Matches)
                    {
                        msg += firstIndex ? $"{match} " : $" / {match}";

                        firstIndex = true;
                    }

                    msg += $": {voteOption.Label}";
                }

                if (m_Config.VotingMode == EVotingMode.PERCENTAGE)
                    msg += "\nVotes will affect the chance for one of the effects to occur.";

                foreach (var votingReceiver in m_VotingReceivers)
                    await votingReceiver.SendMessage(msg);

                break;
            case EOverlayMode.OVERLAY_OBS:
                m_OverlayServer?.NewVoting(m_ActiveVoteOptions);
                break;
            }
            // Clear the old voted for information
            m_UserVotedFor.Clear();
            // Increase the vote counter
            m_VoteCounter++;

            // Vote round started now
            m_VoteRunning = true;
        }
        /// <summary>
        /// Is called when the chaos mod stars a no voting round (callback)
        /// </summary>
        private void OnNoVotingRound(object? sender, EventArgs e)
        {
            m_OverlayServer?.NoVotingRound();
        }
        /// <summary>
        /// Is called when the voting receiver receives a message
        /// </summary>
        private void OnVoteReceiverMessage(object? sender, OnMessageArgs e)
        {
            if (!m_VoteRunning || e.ClientId is null || e.Message is null)
                return;

            if (m_Config.PermittedUsernames?.Length > 0 && e.Username is not null)
            {
                bool found = false;

                foreach (var name in m_Config.PermittedUsernames)
                {
                    // both have already been lowered in other methods, so this comparison is always case-insensitive
                    if (name == e.Username)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    return;
            }

            for (int i = 0; i < m_ActiveVoteOptions.Count; i++)
            {
                var voteOption = m_ActiveVoteOptions[i];

                if (voteOption.Matches.Contains(e.Message))
                {
                    // Check if the player has already voted
                    if (!m_UserVotedFor.TryGetValue(e.ClientId, out int previousVote))
                    {
                        // If they haven't voted, count his vote
                        m_UserVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;

                    }
                    else if (previousVote != i)
                    {
                        // If the player has already voted, and it's not the same as before,
                        // remove the old vote, and add the new one.
                        m_UserVotedFor.Remove(e.ClientId);
                        m_ActiveVoteOptions[previousVote].Votes--;

                        m_UserVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;
                    }

                    break;
                }
            }
        }
    }
}
