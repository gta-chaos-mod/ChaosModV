using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TwitchLib.Api.Models.v5.Clips;

namespace TwitchChatVotingProxy.Properties
{
    class MVoteOption
    {
        public string label { get; set; }
        public int votes { get; set; }
        public string[] matches { get; set; }
    }
    class Message
    {
        public string request { get; set; }
        public MVoteOption[] voteOptions { get; set; }
        public int totalVotes { get; set; }

    }

    class DisplayWebsocket : IDisplayWebsocket
    {
        private List<Fleck.IWebSocketConnection> connections = new List<Fleck.IWebSocketConnection>();
        private List<IVoteOption> activeVote = new List<IVoteOption>();
        
        public DisplayWebsocket(string URL)
        {
            Fleck.WebSocketServer WSS = new Fleck.WebSocketServer(URL);

            WSS.Start(connection =>
            {
                // Add new open connections to the list
                connection.OnOpen += () => this.connections.Add(connection);
                // Remove closed connections from the list
                connection.OnClose += () => this.connections.Remove(connection);
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

        private void broadcast(Message msg)
        {
            this.connections.ForEach(connection => connection.Send(JsonConvert.SerializeObject(msg)));
        }
        private void request(string request, List<IVoteOption> voteOptions)
        {
            Message msg = new Message();
            msg.request = request;
            msg.totalVotes = 0;
            msg.voteOptions = voteOptions.ConvertAll(voteOption =>
            {
                MVoteOption data = new MVoteOption();
                data.label = voteOption.Label;
                data.votes = voteOption.Votes;
                data.matches = voteOption.Matches.ToArray();
                msg.totalVotes += voteOption.Votes;
                return data;
            }).ToArray();
            this.broadcast(msg);
        }
    }
}
