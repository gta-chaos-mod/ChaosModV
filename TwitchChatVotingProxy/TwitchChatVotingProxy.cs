using Serilog;
using Shared;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.OverlayServer;
using TwitchChatVotingProxy.VotingReceiver;

namespace TwitchChatVotingProxy
{
    class TwitchChatVotingProxy
    {
        private static readonly ILogger m_Logger = Log.Logger.ForContext<TwitchChatVotingProxy>();

        private static async Task InitVotingReceivers(List<(string Name, IVotingReceiver VotingReceiver)> votingReceivers, ChaosPipeClient chaosPipe, CancellationTokenSource tokenSource)
        {
            foreach (var votingReceiver in votingReceivers)
            {
                m_Logger.Information($"Initializing {votingReceiver.Name} voting");

                try
                {
                    if (!await votingReceiver.VotingReceiver.Init())
                    {
                        m_Logger.Fatal($"Failed to initialize {votingReceiver.Name} voting");

                        tokenSource.Cancel();
                    }
                }
                catch (Exception exception)
                {
                    m_Logger.Fatal($"Failed to initialize {votingReceiver.Name} voting\nException occured: ${exception}");
                    chaosPipe.SendErrorMessage($"Error occured while initializing {votingReceiver.Name} voting." +
                        $" Check chaosproxy.log for details.");

                    tokenSource.Cancel();
                }
            }
        }

        private static async Task Main(string[] args)
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
               .WriteTo.File("chaosmod/chaosproxy.log",
                    outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
               .CreateLogger();

            m_Logger.Information("===============================");
            m_Logger.Information("Starting chaos mod twitch proxy");
            m_Logger.Information("===============================");

            var config = new OptionsFile("chaosmod/configs/voting.json", "chaosmod/configs/voting.ini", "chaosmod/configs/twitch.ini", "chaosmod/twitch.ini");
            config.ReadFile();

            var mutex = new Mutex(false, "ChaosModVVotingMutex");
            mutex.WaitOne();

            var votingMode = (EVotingMode)config.ReadValue("VotingChanceSystem", 0, "TwitchVotingChanceSystem");
            var overlayMode = (EOverlayMode)config.ReadValue("VotingOverlayMode", 0, "TwitchVotingOverlayMode");
            var retainInitialVotes = config.ReadValue("VotingChanceSystemRetainChance", false, "TwitchVotingChanceSystemRetainChance");

            // Check if OBS overlay should be shown
            OverlayServer.OverlayServer? overlayServer = null;
            if (overlayMode == EOverlayMode.OVERLAY_OBS)
            {
                // Create component
                var overlayServerPort = config.ReadValue("OverlayServerPort", 9091);
                var overlayServerConfig = new OverlayServerConfig(votingMode, retainInitialVotes, overlayServerPort);
                overlayServer = new OverlayServer.OverlayServer(overlayServerConfig);
            }

            // Create components
            var chaosPipe = new ChaosPipeClient();

            var votingReceivers = new List<(string Name, IVotingReceiver VotingReceiver)>();
            if (config.ReadValue("EnableVotingTwitch", false))
                votingReceivers.Add(("Twitch", new TwitchVotingReceiver(config, chaosPipe)));
            if (config.ReadValue("EnableVotingDiscord", false))
                votingReceivers.Add(("Discord", new DiscordVotingReceiver(config, chaosPipe)));

            var tokenSource = new CancellationTokenSource();

            var receiversTask = InitVotingReceivers(votingReceivers, chaosPipe, tokenSource);

            // Start the chaos mod controller
            m_Logger.Information("Initializing controller");

            var permittedUsernames = config.ReadValue("PermittedUsernames", "", "TwitchPermittedUsernames")?.ToLower()
                .Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries).ToArray();
            var chaosModControllerConfig = new ChaosModControllerConfig()
            {
                VotingMode = votingMode,
                OverlayMode = overlayMode,
                RetainInitialVotes = retainInitialVotes,
                PermittedUsernames = permittedUsernames,
                VoteablePrefix = config.ReadValue("VoteablePrefix", "")
            };
            _ = new ChaosModController(chaosPipe, overlayServer, votingReceivers.Select(item => item.VotingReceiver).ToArray(),
                chaosModControllerConfig);

            m_Logger.Information("Sending hello to mod");

            chaosPipe.SendMessageToPipe("hello");
            bool helloBackLogged = false;
            while (chaosPipe.IsConnected() && !tokenSource.IsCancellationRequested)
            {
                if (chaosPipe.GotHelloBack && !helloBackLogged)
                {
                    m_Logger.Information("Received hello_back from mod!");
                    helloBackLogged = true;
                }
                await Task.Delay(100);
            }

            m_Logger.Information("Shutting down");
        }
    }
}
