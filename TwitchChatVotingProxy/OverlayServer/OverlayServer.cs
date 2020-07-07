using Fleck;
using Newtonsoft.Json;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TwitchChatVotingProxy.OverlayServer;
using TwitchLib.Api.Models.v5.Clips;

namespace TwitchChatVotingProxy.Properties
{
    class OverlayServer : IOverlayServer
    {
        private List<Fleck.IWebSocketConnection> connections = new List<Fleck.IWebSocketConnection>();
     
        public OverlayServer(string URL)
        {
            Fleck.WebSocketServer WSS = new Fleck.WebSocketServer(URL);

            WSS.Start(connection =>
            {
                connection.OnOpen += () => OnWsConnectionOption(connection);
                connection.OnClose += () => OnWSConnectionClose(connection);
            });
        }

        public void updateVotes(List<IVoteOption> voteOptions)
        {
            this.request("UPDATE", voteOptions);
        }

        public void createVote(List<IVoteOption> voteOptions)
        {
            this.request("CREATE", voteOptions);
        }

        private void broadcast(OverlayMessage msg)
        {
            this.connections.ForEach(connection => connection.Send(JsonConvert.SerializeObject(msg)));
        }
        private void OnWSConnectionClose(IWebSocketConnection connection)
        {
            Log.Logger.Information($"websocket client disconnected {connection.ConnectionInfo.ClientIpAddress}");
            connections.Remove(connection);
        }
        private void OnWsConnectionOption(IWebSocketConnection connection)
        {
            Log.Logger.Information($"new websocket client {connection.ConnectionInfo.ClientIpAddress}");
            connections.Add(connection);
        }
        private void request(string request, List<IVoteOption> voteOptions)
        {
            var msg = new OverlayMessage();
            msg.request = request;
            msg.voteOptions = voteOptions.ConvertAll(_ => new OverlayVoteOption(_)).ToArray();
            // Count total votes           
            msg.totalVotes = 0;
            voteOptions.ForEach(_ => msg.totalVotes += _.Votes);
            this.broadcast(msg);
        }
    }
}
