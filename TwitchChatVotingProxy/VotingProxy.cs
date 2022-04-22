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
                bool voting = false;

                ChatVotingReceiver twitchVotingReceiver = null;
                ChatVotingReceiver discordVotingReciever = null;
                OverlayServer.OverlayServer overlayServer = null;

                if (config.TwitchVoting)
                {
                    logger.Information("twitch on");

                    VotingReceiverConfig votingReceiverConfig;
                    try
                    {
                        votingReceiverConfig = new VotingReceiverConfig(config.TwitchChannelId, config.TwitchOAuth, config.TwitchUserName, EChannelType.TWITCH);
                    }
                    catch (Exception e)
                    {
                        logger.Fatal(e, "failed to create twitch voting receiver config");
                        return;
                    }

                    // Check if OBS overlay should be shown
                    
                    if (config.TwitchOverlayMode == EOverlayMode.OVERLAY_OBS)
                    {
                        // Create overlay server config
                        OverlayServerConfig overlayServerConfig = new OverlayServerConfig(votingMode, config.RetainInitalVotes, config.ObsOverlayServerPort);

                        // Create component
                        overlayServer = new OverlayServer.OverlayServer(overlayServerConfig);
                    }

                    // Create components
                    twitchVotingReceiver = new ChatVotingReceiver(votingReceiverConfig);

                    voting = true;
                }

                if (config.DiscordVoting)
                {
                    logger.Information("discord on");
                    VotingReceiverConfig votingReceiverConfig;
                    try
                    {
                        votingReceiverConfig = new VotingReceiverConfig(config.DiscordChannelId, config.DiscordOAuth, config.DiscordGuildId, EChannelType.DISCORD);
                    }
                    catch (Exception e)
                    {
                        logger.Fatal(e, "failed to create discord voting receiver config");
                        return;
                    }

                    discordVotingReciever = new ChatVotingReceiver(votingReceiverConfig);

                    voting = true;
                }

                if (voting)
                {
                    var chaosPipe = new ChaosPipeClient();

                    // Start the chaos mod controller
                    new ChaosModController(chaosPipe, overlayServer, twitchVotingReceiver, discordVotingReciever, config);

                    while (chaosPipe.IsConnected()) { }

                    if (twitchVotingReceiver != null)
                    {
                        twitchVotingReceiver.Disconnect();
                    }
                    if (discordVotingReciever != null)
                    {
                        discordVotingReciever.Disconnect();
                    }
                }
                else return;
            }
            finally
            {
                mutex.ReleaseMutex();
            }

            logger.Information("Pipe disconnected, ending program");
        }
    }
}