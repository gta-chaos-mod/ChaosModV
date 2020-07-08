using Serilog;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace TwitchChatVotingProxy.ChaosPipe
{
    class ChaosPipeClient : IChaosPipeClient
    {
        public static readonly int PIPE_TICKRATE = 400;

        public event EventHandler<OnGetVoteResultArgs> OnGetVoteResult;
        public event EventHandler<OnNewVoteArgs> OnNewVote;

        private ILogger logger = Log.Logger.ForContext("class:", typeof(ChaosPipeClient));
        private NamedPipeClientStream pipe = new NamedPipeClientStream(
            ".",
            "ChaosModVTwitchChatPipe",
            PipeDirection.InOut,
            PipeOptions.Asynchronous);
        private StreamReader pipeReader;
        private Timer pipeTick = new Timer();
        private StreamWriter pipeWriter;
        private Task<string> readPipeTask;

        public ChaosPipeClient()
        {
            // Setup pipe tick
            pipeTick.Interval = PIPE_TICKRATE;
            pipeTick.Elapsed += PipeTick;

            // Connect to the chaos mod pipe
            try
            {
                pipe.Connect(1000);
                pipeReader = new StreamReader(pipe);
                pipeWriter = new StreamWriter(pipe);
                pipeWriter.AutoFlush = true;

                logger.Information("successfully connected to chaos mod pipe");

                pipeTick.Enabled = true;
            } catch (Exception e)
            {
                logger.Fatal(e, "failed to connect to chaos mod pipe, aborting");
                return;
            }
        }

        private void GetVoteResult()
        {
            var e = new OnGetVoteResultArgs();
            // Dispatch information to listeners
            OnGetVoteResult.Invoke(this, e);
            // Send the choosen option to the pipe
            if (e.ChoosenOption == null)
            {
                logger.Warning("get vote result did not update choosen option, using 0");
                e.ChoosenOption = 0;
            }
            SendMessageToPipe($"voteresult:{e.ChoosenOption}");
        }
        private void PipeTick(object sender, ElapsedEventArgs e)
        {
            try
            {
                SendHeartBeat();
                ReadPipe();
            } catch(Exception exception)
            {
                logger.Fatal(exception, "chaos mod pipe tick failed, disconnecting");
                pipe.Close();
            }
        }
        private void ReadPipe()
        {
            if (readPipeTask == null) readPipeTask = pipeReader.ReadLineAsync();
            else if (readPipeTask.IsCompleted)
            {
                var message = readPipeTask.Result;
                readPipeTask = null;

                if (message.StartsWith("vote:")) StartNewVote(message);
                else if (mesage == "getvoteresult") GetVoteResult();
                else logger.Warning($"unknown request: {message}");
            }
        }
        private void SendMessageToPipe(string message)
        {
            pipeWriter.Write($"{message}\0");
        }
        private void StartNewVote(string message)
        {
            // Get vote option names (they are separated by ':')
            var optionNames = message.Split(':').ToList();
            // Remove the first option (which is basically the indicator
            // that this is a new vote)
            optionNames.RemoveAt(0);
            // Remove last option (which is not a vote option, but a parameter
            // that we don't use)
            optionNames.RemoveAt(optionNames.Count - 1);
            // Dispatch information to listeners
            OnNewVote.Invoke(this, new OnNewVoteArgs(optionNames.ToArray()));
        }
        private void SendHeartBeat()
        {
            SendMessageToPipe("ping");
        }
    }
}
