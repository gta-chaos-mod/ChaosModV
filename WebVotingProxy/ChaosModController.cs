using ChaosPipe;
using WebVotingProxy.VotingReceiver;

namespace WebVotingProxy
{
    internal class ChaosModController : ChaosControllerBase
    {
        private readonly MasterClient _masterClient;

        public ChaosModController(IChaosPipeClient chaosPipe, MasterClient masterClient, bool retainInitialVotes) 
            : base(chaosPipe, retainInitialVotes)
        {
            _masterClient = masterClient;
            
            chaosPipe.OnGetVoteResult += OnGetVoteResult;
            chaosPipe.OnNewVote += OnNewVote;

            _masterClient.Client.On("game:vote", response =>
            {
                string socketId = response.GetValue<string>();
                int voteIdx = response.GetValue<int>(1);
                OnVoteReceiverMessage(new OnMessageArgs
                {
                    ClientId = socketId, Idx = voteIdx
                });
            });
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
            VoteRunning = false;
        }

        /// <summary>
        ///     Is called when the chaos mod start a new vote (callback)
        /// </summary>
        private void OnNewVote(object sender, OnNewVoteArgs e)
        {
            ActiveVoteOptions = GenerateVotes(e);
            _masterClient.Client.EmitAsync("game:new", _masterClient.RoomId, ActiveVoteOptions);
            UserVotedFor.Clear();
            VoteCounter++;
            VoteRunning = true;
        }

        /// <summary>
        ///     Is called when the voting receiver receives a message
        /// </summary>
        private void OnVoteReceiverMessage(OnMessageArgs e)
        {
            if (!VoteRunning) return;

            var voteOption = ActiveVoteOptions[e.Idx];
            
            if (!UserVotedFor.TryGetValue(e.ClientId, out var previousVote))
            {
                UserVotedFor.Add(e.ClientId, e.Idx);
                voteOption.Votes++;
            }
            else if (previousVote != e.Idx)
            {
                UserVotedFor.Remove(e.ClientId);
                ActiveVoteOptions[previousVote].Votes--;

                UserVotedFor.Add(e.ClientId, e.Idx);
                voteOption.Votes++;
            }
        }
    }
}