using Serilog;
using System;
using System.Threading;
using VotingProxy.ChaosPipe;
using VotingProxy.OverlayServer;
using VotingProxy.VotingReceiver;

namespace VotingProxy
{
    class VotingProxy
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
            logger = Log.Logger.ForContext<VotingProxy>();

            logger.Information("===============================");
            logger.Information("Starting chaos mod voting proxy");
            logger.Information("===============================");
            
            // Read big config file WIP
            var config = new Config.Config("./chaosmod/voting.ini");

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
                VotingReceiverConfig votingReceiverConfig;
                try
                {
                    votingReceiverConfig = new VotingReceiverConfig(config.ChannelId, config.OAuth, config.UserName, config.ChannelType);
                }
                catch (Exception e)
                {
                    logger.Fatal(e, "failed to create voting receiver config");
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
                var votingReceiver = new ChatVotingReceiver(votingReceiverConfig);
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