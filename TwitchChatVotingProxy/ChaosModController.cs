using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Timers;
using ChaosPipe;
using TwitchChatVotingProxy.Config;
using TwitchChatVotingProxy.OverlayServer;
using TwitchChatVotingProxy.VotingReceiver;

namespace TwitchChatVotingProxy
{
    class ChaosModController : ChaosControllerBase
    {
        public static readonly int DISPLAY_UPDATE_TICKRATE = 200;
        
        private IChaosPipeClient chaosPipe;
        private Timer displayUpdateTick = new Timer(DISPLAY_UPDATE_TICKRATE);
        private IOverlayServer? overlayServer;
        private EVotingMode? votingMode;
        private EOverlayMode? overlayMode;
        private IVotingReceiver votingReceiver;

        public ChaosModController(
            IChaosPipeClient chaosPipe,
            IOverlayServer overlayServer,
            IVotingReceiver votingReceiver,
            IConfig config
        ) : base(chaosPipe, config.RetainInitalVotes) {
            this.chaosPipe = chaosPipe;
            this.overlayServer = overlayServer;
            this.votingReceiver = votingReceiver;

            // Setup pipe listeners
            this.chaosPipe.OnGetVoteResult += OnGetVoteResult;
            this.chaosPipe.OnNewVote += OnNewVote;
            this.chaosPipe.OnNoVotingRound += OnNoVotingRound;

            // Setup receiver listeners
            this.votingReceiver.OnMessage += OnVoteReceiverMessage;

            // Setup config options
            votingMode = config.VotingMode;
            overlayMode = config.OverlayMode;

            // Setup display update tick
            displayUpdateTick.Elapsed += DisplayUpdateTick;
            displayUpdateTick.Enabled = true;
        }

        /// <summary>
        /// Does the display update tick and is called by a timer
        /// </summary>
        private void DisplayUpdateTick(object sender, ElapsedEventArgs e)
        {
            overlayServer.UpdateVoting(ActiveVoteOptions);
        }

        /// <summary>
        /// Is called when the chaos mod wants to know the voting result (callback)
        /// </summary>
        private void OnGetVoteResult(object sender, OnGetVoteResultArgs e)
        {
            // Tell the overlay server that the vote has ended
            try
            {
                overlayServer.EndVoting();

            } catch (Exception err)
            {
                Log.Error(err, "error occured");
            }

            // Evaluate what result calculation to use
            switch (votingMode)
            {
                case EVotingMode.MAJORITY:
                    e.ChosenOption = GetVoteResultByMajority();
                    break;
                case EVotingMode.PERCENTAGE:
                    e.ChosenOption = GetVoteResultByPercentage();
                    break;
            }

            // Vote round ended
            VoteRunning = false;
        }
        /// <summary>
        /// Is called when the chaos mod start a new vote (callback)
        /// </summary>
        private void OnNewVote(object sender, OnNewVoteArgs e)
        {
            ActiveVoteOptions = GenerateVotes(e);
            // Depending on the overlay mode either inform the overlay server about the new vote or send a chat message
            switch (overlayMode)
            {
                case EOverlayMode.CHAT_MESSAGES:
                    votingReceiver.SendMessage("Time for a new effect! Vote between:");
                    foreach (IVoteOption voteOption in ActiveVoteOptions)
                    {
                        string msg = string.Empty;

                        bool firstIndex = true;
                        foreach (string match in voteOption.Matches)
                        {
                            msg += firstIndex ? $"{match} " : $" / {match}";

                            firstIndex = true;
                        }

                        msg += $": {voteOption.Label}\n";

                        votingReceiver.SendMessage(msg);
                    }

                    if (votingMode == EVotingMode.PERCENTAGE)
                    {
                        votingReceiver.SendMessage("Votes will affect the chance for one of the effects to occur.");
                    }

                    break;
                case EOverlayMode.OVERLAY_OBS:
                    overlayServer.NewVoting(ActiveVoteOptions);
                    break;
            }
            // Clear the old voted for information
            UserVotedFor.Clear();
            // Increase the vote counter
            VoteCounter++;

            // Vote round started now
            VoteRunning = true;
        }
        /// <summary>
        /// Is called when the chaos mod stars a no voting round (callback)
        /// </summary>
        private void OnNoVotingRound(object sender, EventArgs e)
        {
            overlayServer.NoVotingRound();
        }
        /// <summary>
        /// Is called when the voting receiver receives a message
        /// </summary>
        private void OnVoteReceiverMessage(object sender, OnMessageArgs e)
        {
            if (!VoteRunning) return;

            for (int i = 0; i < ActiveVoteOptions.Count; i++)
            {
                var voteOption = ActiveVoteOptions[i];

                if (voteOption.Matches.Contains(e.Message))
                {
                    int previousVote;

                    // Check if the player has already voted
                    if (!UserVotedFor.TryGetValue(e.ClientId, out previousVote))
                    {
                        // If they haven't voted, count his vote
                        UserVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;
       
                    } else if (previousVote != i)
                    {
                        // If the player has already voted, and it's not the same as before,
                        // remove the old vote, and add the new one.
                        UserVotedFor.Remove(e.ClientId);
                        ActiveVoteOptions[previousVote].Votes--;

                        UserVotedFor.Add(e.ClientId, i);
                        voteOption.Votes++;
                    }

                    break;
                }
            }
        }
    }
}
