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

namespace TwitchChatVotingProxy.OverlayServer
{
    class OverlayServer : IOverlayServer
    {
        private List<Fleck.IWebSocketConnection> connections = new List<Fleck.IWebSocketConnection>();
        private ILogger logger = Log.Logger.ForContext<OverlayServer>();
        private EVotingMode votingMode;

        public OverlayServer(string URL, EVotingMode votingMode)
        {
            this.votingMode = votingMode;

            // Start the websocket
            Fleck.WebSocketServer WSS = new Fleck.WebSocketServer(URL);
            // Set the websocket listeners
            WSS.Start(connection =>
            {
                connection.OnOpen += () => OnWsConnectionOption(connection);
                connection.OnClose += () => OnWSConnectionClose(connection);
            });
        }

        public void EndVoting()
        {
            Request("END", new List<IVoteOption>());
        }
        /// <summary>
        /// Notifies clients that a new vote started
        /// </summary>
        /// <param name="voteOptions">New vote options</param>
        public void NewVoting(List<IVoteOption> voteOptions)
        {
            Request("CREATE", voteOptions);
        }
        /// <summary>
        /// Is called when a client disconnects from the websocket
        /// </summary>
        /// <param name="connection">The client that disconnected</param>
        private void OnWSConnectionClose(IWebSocketConnection connection)
        {
            logger.Information($"websocket client disconnected {connection.ConnectionInfo.ClientIpAddress}");
            connections.Remove(connection);
        }
        /// <summary>
        /// Is called when a new client connects to the websocket
        /// </summary>
        /// <param name="connection">The client that connected</param>
        private void OnWsConnectionOption(IWebSocketConnection connection)
        {
            logger.Information($"new websocket client {connection.ConnectionInfo.ClientIpAddress}");
            connections.Add(connection);
        }
        /// <summary>
        /// Sends a request to the clients
        /// </summary>
        /// <param name="request">Name of the request</param>
        /// <param name="voteOptions">Vote options that should be sent</param>
        private void Request(string request, List<IVoteOption> voteOptions)
        {
            var msg = new OverlayMessage();
            msg.request = request;
            msg.voteOptions = voteOptions.ConvertAll(_ => new OverlayVoteOption(_)).ToArray();
            var strVotingMode = "";
            if (VotingModeDict.Dict.TryGetValue(this.votingMode, out strVotingMode))
            {
                msg.votingMode = strVotingMode;
            } else
            {
                logger.Error($"could not find voting mode {this.votingMode} in dictonary");
                msg.votingMode = "UNKNOWN_VOTING_MODE";
            }
            // Count total votes           
            msg.totalVotes = 0;
            voteOptions.ForEach(_ => msg.totalVotes += _.Votes);
            // Send the message to all clients
            connections.ForEach(connection => connection.Send(JsonConvert.SerializeObject(msg)));
        }
        /// <summary>
        /// Notifies clients about vote updates
        /// </summary>
        /// <param name="voteOptions">Updated vote options</param>
        public void UpdateVoting(List<IVoteOption> voteOptions)
        {
            Request("UPDATE", voteOptions);
        }
    }
}


