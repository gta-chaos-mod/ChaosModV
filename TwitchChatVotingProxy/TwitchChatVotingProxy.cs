using Serilog;
using System;
using System.Threading;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.OverlayServer;
using TwitchChatVotingProxy.VotingReceiver;

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        private static ILogger logger;

        private static void Main(string[] args)
        {
            if (args.Length < 1 || args[0] != "--startProxy")
            {
                Console.WriteLine("Please don't start the voting proxy process manually as it's only supposed to be launched by the mod itself."
                    + "\nPass --startProxy as an argument if you want to start the proxy yourself for debugging purposes.");

                Console.ReadKey();

                return;
            }

            Log.Logger = new LoggerConfiguration()
               .MinimumLevel.Debug()
               .WriteTo.File("./chaosmod/chaosProxy.log", outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
               .CreateLogger();
            logger = Log.Logger.ForContext<TwitchChatVotingProxy>();

            logger.Information("===============================");
            logger.Information("Starting chaos mod twitch proxy");
            logger.Information("===============================");
            
            // Read big config file WIP
            var config = new Config.Config("./chaosmod/twitch.ini");

            // Validate voting mode
            EVotingMode votingMode;
            if (config.VotingMode == null)
            {
                votingMode = EVotingMode.MAJORITY;
                logger.Warning($"voting mode is null, using default \"{VotingMode.Lookup(votingMode)}\"");
            }
            else votingMode = (EVotingMode)config.VotingMode;

            Mutex mutex = new Mutex(false, "ChaosModVVotingMutex");
            mutex.WaitOne();

            try
            {
                // Create twitch config
                TwitchVotingReceiverConfig twitchVotingReceiverConfig;
                try
                {
                    twitchVotingReceiverConfig = new TwitchVotingReceiverConfig(config.TwitchChannelName, config.TwitchOAuth, config.TwitchUserName);
                }
                catch (Exception e)
                {
                    logger.Fatal(e, "failed to create twitch voting receiver config");
                    return;
                }

                // Check if OBS overlay should be shown
                OverlayServer.OverlayServer overlayServer = null;
                if (config.OverlayMode == EOverlayMode.OVERLAY_OBS)
                {
                    // Create overlay server config
                    OverlayServerConfig overlayServerConfig = new OverlayServerConfig(votingMode, config.RetainInitalVotes, config.OverlayServerPort);

                    // Create component
                    overlayServer = new OverlayServer.OverlayServer(overlayServerConfig);
                }

                // Create components
                var votingReceiver = new TwitchVotingReceiver(twitchVotingReceiverConfig);
                var chaosPipe = new ChaosPipeClient();

                // Start the chaos mod controller
                new ChaosModController(chaosPipe, overlayServer, votingReceiver, config);

                while (chaosPipe.IsConnected()) { }
            }
            finally
            {
                mutex.ReleaseMutex();
            }

            logger.Information("Pipe disconnected, ending program");
        }
    }
}