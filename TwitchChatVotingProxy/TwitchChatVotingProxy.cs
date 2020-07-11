using Serilog;
using System;
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

            Log.Logger = new LoggerConfiguration()
               .MinimumLevel.Debug()
               .WriteTo.File("./chaosmod/chaosProxy.log", outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
               .CreateLogger();
            logger = Log.Logger.ForContext<TwitchChatVotingProxy>();

            logger.Information("===============================");
            logger.Information("Starting chaos mod twtich proxy");
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

            // Create twitch config
            TwitchVotingReceiverConfig twitchVotingReceiverConfig;
            try
            {
                 twitchVotingReceiverConfig = new TwitchVotingReceiverConfig(config.TwitchChannelName, config.TwitchOAuth, config.TwitchUserName);
            } catch (Exception e)
            {
                logger.Fatal(e, "failed to create twitch voting receiver config");
                return;
            }

            // Create overlay server config
            OverlayServerConfig overlayServerConfig = new OverlayServerConfig(votingMode, config.OverlayServerPort);

            // Create components
            var overlayServer = new OverlayServer.OverlayServer(overlayServerConfig);
            var votingReceiver = new TwitchVotingReceiver(twitchVotingReceiverConfig);
            var chaosPipe = new ChaosPipeClient();

            // Start the chaos mod controller
            new ChaosModController(chaosPipe, overlayServer, votingReceiver);

            while (chaosPipe.IsConnected()) { }

            logger.Information("Pipe disconnected, ending program");
        }
    }
}