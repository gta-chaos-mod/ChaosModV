using System;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Threading.Tasks;
using System.Timers;
using Serilog;

namespace ChaosPipe
{
    public class ChaosPipeClient : IChaosPipeClient
    {
        /// <summary>
        ///     Speed at which the chaos mod pipe gets processed
        /// </summary>
        public static readonly int PIPE_TICKRATE = 100;

        private readonly ILogger _logger = Log.Logger.ForContext<ChaosPipeClient>();
        private readonly NamedPipeClientStream _pipe = new NamedPipeClientStream(
            ".",
            "ChaosModVProxyChatPipe",
            PipeDirection.InOut,
            PipeOptions.Asynchronous);
        private StreamReader _pipeReader;
        private readonly Timer _pipeTick = new Timer();
        private StreamWriter _pipeWriter;
        private Task<string> _readPipeTask;

        public ChaosPipeClient()
        {
            // Setup pipe tick
            _pipeTick.Interval = PIPE_TICKRATE;
            _pipeTick.Elapsed += PipeTick;
        }

        public void Connect()
        {
            try
            {
                _pipe.Connect(5000);
                _pipeReader = new StreamReader(_pipe);
                _pipeWriter = new StreamWriter(_pipe) { AutoFlush = true };
                _pipeTick.Enabled = true;
            }
            catch
            {
                //Ignore
            }
        }

        public event EventHandler<OnGetCurrentVotesArgs> OnGetCurrentVotes;
        public event EventHandler<OnGetVoteResultArgs> OnGetVoteResult;
        public event EventHandler<OnNewVoteArgs> OnNewVote;
        public event EventHandler OnNoVotingRound;

        /// <summary>
        ///     Check if the chaos mod pipe is still connected
        /// </summary>
        /// <returns>If the chaos mod pipe is still connected</returns>
        public bool IsConnected()
        {
            return _pipe.IsConnected;
        }

        /// <summary>
        ///     Disconnects the stream reader/writer and the pipe itself
        /// </summary>
        private void DisconnectFromPipe()
        {
            _pipeReader.Close();
            _pipeWriter.Close();
            _pipe.Close();
        }

        private void GetCurrentVotes()
        {
            var args = new OnGetCurrentVotesArgs();
            OnGetCurrentVotes?.Invoke(this, args);
            if (args.CurrentVotes == null)
            {
                _logger.Error("listeners failed to supply on get current vote args");
            }
            else
            {
                var currentVotes = string.Join(":", args.CurrentVotes.Select(_ => _.ToString()).ToArray());
                SendMessageToPipe($"currentvotes:{currentVotes}");
            }
        }

        /// <summary>
        ///     Gets called when the chaos mod requests vote results
        /// </summary>
        private void GetVoteResult()
        {
            _logger.Debug("asking listeners for vote result");
            var e = new OnGetVoteResultArgs();
            OnGetVoteResult?.Invoke(this, e);
            if (e.ChosenOption == null)
            {
                _logger.Warning("get vote result did not update chosen option, using 0 (first option)");
                e.ChosenOption = 0;
            }

            SendMessageToPipe($"voteresult:{e.ChosenOption}");
            _logger.Debug($"vote result sent to pipe: {e.ChosenOption}");
        }

        /// <summary>
        ///     Gets called every pipe tick
        /// </summary>
        private void PipeTick(object sender, ElapsedEventArgs e)
        {
            try
            {
                SendHeartBeat();
                ReadPipe();
            }
            catch
            {
                DisconnectFromPipe();
            }
        }

        /// <summary>
        ///     Reads the contents of the chaos mod pipe and evaluates its message
        /// </summary>
        private void ReadPipe()
        {
            // If no reading task is active, create one
            if (_readPipeTask == null)
            {
                _readPipeTask = _pipeReader.ReadLineAsync();
            }
            // If the reading task is created and complete, get its results
            else if (_readPipeTask.IsCompleted)
            {
                // Get the message from the pipe read
                var message = _readPipeTask.Result;
                // Null the reading task so the next read is dispatched
                _readPipeTask = null;

                // Evaluate message
                if (string.IsNullOrEmpty(message)) return;
                if (message.StartsWith("vote:")) StartNewVote(message);
                else switch (message)
                {
                    case "getvoteresult":
                        GetVoteResult();
                        break;
                    case "novoteround":
                        StartNoVotingRound();
                        break;
                    case "getcurrentvotes":
                        GetCurrentVotes();
                        break;
                    default:
                        _logger.Warning($"unknown request: {message}");
                        break;
                }
            }
        }

        /// <summary>
        ///     Sends a message to the chaos mod pipe
        /// </summary>
        /// <param name="message">Message to be sent</param>
        private void SendMessageToPipe(string message)
        {
            try
            {
                _pipeWriter.Write($"{message}\0");
            }
            catch
            {
                DisconnectFromPipe();
            }
        }

        /// <summary>
        ///     Is called when the chaos mod starts a new vote
        /// </summary>
        /// <param name="message">Message from the pipe to be parsed into votes</param>
        private void StartNewVote(string message)
        {
            // Get vote option names (they are separated by ':')
            var optionNames = message.Split(':').ToList();
            // Remove the first option (which is basically the indicator
            // that this is a new vote)
            optionNames.RemoveAt(0);
            // Dispatch information to listeners
            OnNewVote?.Invoke(this, new OnNewVoteArgs(optionNames.ToArray()));
        }

        /// <summary>
        ///     Start a no-voting round. The chaos mod will decide over the options
        /// </summary>
        private void StartNoVotingRound()
        {
            OnNoVotingRound?.Invoke(this, null);
        }

        /// <summary>
        ///     Sends a heartbeat to the chaos mod
        /// </summary>
        private void SendHeartBeat()
        {
            SendMessageToPipe("ping");
        }
    }
}