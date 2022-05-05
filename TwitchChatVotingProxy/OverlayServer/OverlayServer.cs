using Fleck;
using Newtonsoft.Json;
using Serilog;
using System;
using System.Collections.Generic;

// TODO: fix voting mode
namespace TwitchChatVotingProxy.OverlayServer
{
    class OverlayServer : IOverlayServer
    {
        private OverlayServerConfig config;
        private List<Fleck.IWebSocketConnection> connections = new List<Fleck.IWebSocketConnection>();
        private ILogger logger = Log.Logger.ForContext<OverlayServer>();

        public OverlayServer(OverlayServerConfig config)
        {
            this.config = config;

            try
            {
                var WSS = new Fleck.WebSocketServer($"ws://0.0.0.0:{config.Port}");
                // Set the websocket listeners
                WSS.Start(connection =>
                {
                    connection.OnOpen += () => OnWsConnectionOpen(connection);
                    connection.OnClose += () => OnWSConnectionClose(connection);
                });
            } catch (Exception e)
            {
                logger.Fatal(e, "failed so start websocket server");
            }
        }

        public void EndVoting()
        {
            Request("END", new List<IVoteOption>());
        }
        public void NewVoting(List<IVoteOption> voteOptions)
        {
            Request("CREATE", voteOptions);
        }
        public void NoVotingRound()
        {
            Request("NO_VOTING_ROUND", new List<IVoteOption>());
        }
        public void UpdateVoting(List<IVoteOption> voteOptions)
        {
            Request("UPDATE", voteOptions);
        }

        /// <summary>
        /// Broadcasts a message to all socket clients
        /// </summary>
        /// <param name="message">Message which should be broadcast</param>
        private void Broadcast(string message)
        {
            connections.ForEach(connection =>
            {
                // If the connection is not available for some reason, we just close it
                if (!connection.IsAvailable) connection.Close();
                else connection.Send(message);
            });
        }
        /// <summary>
        /// Is called when a client disconnects from the websocket
        /// </summary>
        /// <param name="connection">The client that disconnected</param>
        private void OnWSConnectionClose(IWebSocketConnection connection)
        {
            try
            {
                logger.Information($"websocket client disconnected {connection.ConnectionInfo.ClientIpAddress}");
                connections.Remove(connection);
            } catch (Exception e)
            {
                logger.Error(e, "error occurred as client disconnected");
            }
        }
        /// <summary>
        /// Is called when a new client connects to the websocket
        /// </summary>
        /// <param name="connection">The client that connected</param>
        private void OnWsConnectionOpen(IWebSocketConnection connection)
        {
            try
            {
                logger.Information($"new websocket client {connection.ConnectionInfo.ClientIpAddress}");
                connections.Add(connection);
            } catch (Exception e)
            {
                logger.Error(e, "error occurred as client connected");
            }
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
            msg.retainInitialVotes = config.RetainInitialVotes;
            var strVotingMode = VotingMode.Lookup(config.VotingMode);
            if (strVotingMode != null)
            {
                msg.votingMode = strVotingMode;
            } else
            {
                logger.Error($"could not find voting mode {config.VotingMode} in dictionary");
                msg.votingMode = "UNKNOWN_VOTING_MODE";
            }
            // Count total votes      
            msg.totalVotes = 0;
            voteOptions.ForEach(_ => msg.totalVotes += _.Votes);
            // Send the message to all clients
            Broadcast(JsonConvert.SerializeObject(msg));
        }
    }
}


