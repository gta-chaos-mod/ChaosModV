using System;
using Serilog;
using WebVotingProxy.ChaosPipe;
using WebVotingProxy.VotingReceiver;

namespace WebVotingProxy
{
    internal class Program
    {
        private static ILogger _logger;

        private static void Main()
        {
            Console.Title = "Chaos Mod V - WebProxy (c) 2021 DasDarki";
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File("./chaosmod/chaosProxy.log", outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
                .CreateLogger();
            _logger = Log.Logger.ForContext<Program>();

            _logger.Information("============================");
            _logger.Information("Starting chaos mod web proxy");
            _logger.Information("============================");

            var masterClient = new MasterClient();
            if (!masterClient.Connect().GetAwaiter().GetResult())
            {
                _logger.Error("Connection to the master server failed!");
                return;
            }

            masterClient.Client.EmitAsync("room:request").GetAwaiter().GetResult();
            var chaosPipe = new ChaosPipeClient();
            while (true)
            {
                chaosPipe.Connect();
                if (chaosPipe.IsConnected())
                {
                    _logger.Information("Successfully conntect to ChaosModV!");
                    _logger.Information("");
                    _logger.Warning("   Your Chaos-Token: {Token}", masterClient.RoomId);
                    _logger.Information("");
                    new ChaosModController(chaosPipe, masterClient, false);
                    while (chaosPipe.IsConnected()) { }
                    _logger.Fatal("Lost connection");
                }
            }
        }
    }
}
