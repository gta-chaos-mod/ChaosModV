using Serilog;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.VotingReceiver;

/**
 * TODO: Choose random option when multiple options have the same votes
 */

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        private static ILogger logger;

        private static void Main(string[] args)
        {

            Log.Logger = new LoggerConfiguration()
               .MinimumLevel.Information()
               .Enrich.FromLogContext()
               .WriteTo.File("./chaosmod/chaosProxy.log", outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
               .CreateLogger();

            logger = Log.Logger.ForContext<TwitchChatVotingProxy>();

            logger.Information("========================================");
            logger.Information("Starting chaos mod twtich proxy");
            logger.Information("========================================");
                    
            var twitchConfig = new TwitchConfig.TwitchConfig("./chaosmod/twitch.ini");
            EVotingMode votingMode;

            // TODO: "twitch config" should become a global config, that also stores
            // username for potential youtube clients and other stuff.
            // this validation could then be moved to each individual voting receiver
            // Validate config
            if (twitchConfig.ChannelName == null)
            {
                logger.Fatal("twitch channel name is null, aborting");
                return;
            }
            if (twitchConfig.OAuth == null)
            {
                logger.Fatal("twitch oAuth is null, aborting");
                return;
            }
            if (twitchConfig.UserName == null)
            {
                logger.Fatal("twitch user name is null, aborting");
                return;
            }
            if (twitchConfig.VotingMode == null)
            {
                votingMode = EVotingMode.PERCENTAGE;
                logger.Warning($"twitch voting mode is null, using default \"{VotingModeDict.Lookup(votingMode)}\"");
            }
            else votingMode = (EVotingMode)twitchConfig.VotingMode;

            var overlayServer = new OverlayServer.OverlayServer("ws://127.0.0.1:9091", votingMode);
            var votingReceiver = new TwitchVotingReceiver(twitchConfig);
            var chaosPipe = new ChaosPipeClient();

            var controller = new ChaosModController(chaosPipe, overlayServer, votingReceiver);


            while (chaosPipe.IsConnected()) { }

            logger.Information("Pipe disconnected, ending programm");
        }
    }
}