using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Timers;
using TwitchChatVotingProxy.Properties;

namespace TwitchChatVotingProxy
{
    class GetVoteResultEventArgs : EventArgs
    {
        public int SelectedOption { get; set; }
    }
    class ChaosPipeClient
    {
        public static readonly int PIPE_RECONNECT_TIMEOUT = 1000;
        public static readonly int TICK_RATE = 500;

        // TODO: update this so it has proper event args es event args (more like OnGetVoteResults).
        public event EventHandler<List<IVoteOption>> OnNewVote;
        public event EventHandler<GetVoteResultEventArgs> OnGetVoteResult;

        private NamedPipeClientStream pipe = new NamedPipeClientStream(".", "ChaosModVTwitchChatPipe", PipeDirection.InOut, PipeOptions.Asynchronous);
        private StreamReader pipeReader;
        private StreamWriter pipeWriter;
        private Timer pipeTickInterval = new Timer();
        private Task<string> readPipeTask;
        private int voteCounter = 0;

        public ChaosPipeClient()
        {      
            pipeTickInterval.Interval = TICK_RATE;
            pipeTickInterval.Elapsed += PipeTick;

            connect();
        } 

        private void connect()
        {
            try
            {
                pipe.Connect(PIPE_RECONNECT_TIMEOUT);
            } catch (Exception e)
            {
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine("failed to connect to chaos mod pipe");
            }

            if (!pipe.IsConnected)
            {
                Console.WriteLine($"reconnecting to chaos mod in {PIPE_RECONNECT_TIMEOUT}ms");
                Task.Run(async () =>
                {
                    await Task.Delay(PIPE_RECONNECT_TIMEOUT);
                    connect();
                });
            } else
            {
                Console.WriteLine("successfully connecte to chaos mod pipe");
                pipeReader = new StreamReader(pipe);
                pipeWriter = new StreamWriter(pipe);
                pipeWriter.AutoFlush = true;
                pipeTickInterval.Enabled = true;
            }
        }

        private void GetVoteResult()
        {
            var e = new GetVoteResultEventArgs();
            // Set the default selected option to 0
            e.SelectedOption = 0;
            // Dispatch the event (the user is now supposed to update the selected option)
            OnGetVoteResult.Invoke(this, e);
            // Send to the pipe which option was selected
            SendMessage($"voteresult:{e.SelectedOption}");
        }
        private void StartNewVote(string message)
        {
            var optionsNames = message.Split(':').ToList();
            // remove the first element (vote) and the last element (0/1)
            // which are not part of the option
            optionsNames.RemoveAt(0);
            optionsNames.RemoveAt(optionsNames.Count - 1);
            // Convert the options names to options
            var options = new List<IVoteOption>();

            for (var i = 0; i < optionsNames.Count; i++)
            {
                // Alternate between index and index + options length.
                // This has the effect that people don't need to type messages
                // twice, which is prevented by some chat implementations (like twitch).
                string MATCH = ((voteCounter % 2 == 0 
                    // If the vote count is even, use the index (+1 because normal humans)
                    ? i 
                    // If the vote count is odd use the index (+1) plus the total option length.
                    : i + optionsNames.Count) + 1).ToString();
                
                var matches = new List<string>(){ MATCH };
                options.Add(new VoteOption(optionsNames[i], matches));
            }
            // Increase the vote counter
            voteCounter++;
            // Dispatch update to listeners
            OnNewVote.Invoke(this, options);
        }
        private void PipeTick(object a, ElapsedEventArgs b)
        {
            // Exceptions are not thrown as this, as the code is called asynchronously
            try
            {
                // If the pipe disconnected
                if (!pipe.IsConnected)
                {
                    pipeTickInterval.Enabled = false;
                    connect();
                } else
                {
                    // Send heartbeat
                    SendMessage("ping");
                    if (readPipeTask == null) readPipeTask = pipeReader.ReadLineAsync();
                    else if (readPipeTask.IsCompleted)
                    {
                        var message = readPipeTask.Result;
                        if (message.StartsWith("vote:")) StartNewVote(message);
                        else if (message == "getvoteresult") GetVoteResult();
                        else Console.WriteLine($"unknown request: {message}");

                        readPipeTask = null;
                    }
                }
            } catch (Exception e)
            {
                Console.Write(e);
            }
        }
        private void SendMessage(string message)
        {
            if (pipeWriter == null)
            {
                throw new NullReferenceException("cannot send message when pipe writer is null");
            }
            else if (!pipe.IsConnected)
            {
                throw new Exception("cannot send message when pipe is not connected");
            }
            pipeWriter.Write($"{message}\0");
        }
    }
}
