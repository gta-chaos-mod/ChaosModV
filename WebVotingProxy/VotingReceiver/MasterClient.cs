using System;
using System.Threading.Tasks;
using Serilog;
using SocketIOClient;

namespace WebVotingProxy.VotingReceiver
{
    /// <summary>
    /// The master client connects to the WebProxy server for communication with the web panel.
    /// </summary>
    internal class MasterClient
    {
        internal SocketIO Client { get; }
        
        internal string RoomId { get; private set; }

        private readonly ILogger _logger;

        internal MasterClient()
        {
            _logger = Log.Logger.ForContext<MasterClient>();
            Client = new SocketIO("http://localhost:45811/", new SocketIOOptions
            {
                Reconnection = true
            });
            Client.On("room:send", response =>
            {
                RoomId = response.GetValue<string>();
            });
        }

        internal async Task<bool> Connect()
        {
            try
            {
                await Client.ConnectAsync();
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}