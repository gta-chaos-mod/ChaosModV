using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.WebSockets;
using System.Threading;
using Serilog;
using Newtonsoft.Json.Linq;

namespace TwitchChatVotingProxy.VotingReceiver
{
    public class StreamerBotVotingReceiver : IVotingReceiver
    {
        public event EventHandler<OnMessageArgs> OnMessage;

        private ClientWebSocket ws = new ClientWebSocket();
        private CancellationTokenSource cts = new CancellationTokenSource();
        private readonly ILogger m_Logger = Log.Logger.ForContext<StreamerBotVotingReceiver>();

        private readonly string streamerBotWsUrl;

        public StreamerBotVotingReceiver(int port)
        {
            streamerBotWsUrl = $"ws://127.0.0.1:{port}/";
        }

        public async Task<bool> Init()
        {
            m_Logger.Information("Connecting to Streamer.bot WebSocket");
            try
            {
                await ws.ConnectAsync(new Uri(streamerBotWsUrl), cts.Token);
                m_Logger.Information("Connected to Streamer.bot WebSocket");

                await SubscribeToYouTubeMessages();

                _ = Task.Run(Listen);
                return true;
            }
            catch (Exception ex)
            {
                m_Logger.Error(ex, "Failed to connect to Streamer.bot WebSocket");
                return false;
            }
        }

        private async Task SubscribeToYouTubeMessages()
        {
            var subscriptionRequest = new
            {
                request = "Subscribe",
                events = new
                {
                    YouTube = new[] { "Message" }
                },
                id = "chaosmod-youtube-message-subscription"
            };

            var jsonRequest = Newtonsoft.Json.JsonConvert.SerializeObject(subscriptionRequest);
            var bytes = Encoding.UTF8.GetBytes(jsonRequest);
            await ws.SendAsync(new ArraySegment<byte>(bytes), WebSocketMessageType.Text, true, CancellationToken.None);
            m_Logger.Information("Subscribed to Streamer.bot YouTube Message events");
        }

        public Task SendMessage(string msg)
        {
            // Implement sending message via Streamer.bot if needed.
            // This might involve crafting a specific JSON message for Streamer.bot to send a chat message.
            m_Logger.Information($"StreamerBotVotingReceiver: SendMessage not implemented. Message: {msg}");
            return Task.CompletedTask;
        }

        private async Task Listen()
        {
            var buffer = new byte[1024 * 4];
            while (ws.State == WebSocketState.Open)
            {
                var result = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                if (result.MessageType == WebSocketMessageType.Close)
                {
                    await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
                }
                else
                {
                    var message = Encoding.UTF8.GetString(buffer, 0, result.Count);
                    m_Logger.Debug($"Received message from Streamer.bot: {message}");

                    try
                    {
                        var json = JObject.Parse(message);
                        if (json["event"]?["source"]?.ToString() == "YouTube" && json["event"]?["type"]?.ToString() == "Message")
                        {
                            var username = json["data"]?["message"]?["displayName"]?.ToString();
                            var text = json["data"]?["message"]?["message"]?.ToString();

                            if (!string.IsNullOrEmpty(username) && !string.IsNullOrEmpty(text))
                            {
                                OnMessage?.Invoke(this, new OnMessageArgs
                                {
                                    ClientId = username, // Using username as client id
                                    Username = username,
                                    Message = text
                                });
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        m_Logger.Error(ex, "Failed to parse message from Streamer.bot");
                    }
                }
            }
        }
    }
}
