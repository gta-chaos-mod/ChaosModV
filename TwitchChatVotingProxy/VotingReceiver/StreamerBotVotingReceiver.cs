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
            var buffer = new ArraySegment<byte>(new byte[8192]); // Use a larger buffer
            
            while (ws.State == WebSocketState.Open)
            {
                using (var ms = new System.IO.MemoryStream())
                {
                    WebSocketReceiveResult result;
                    do
                    {
                        result = await ws.ReceiveAsync(buffer, CancellationToken.None);
                        ms.Write(buffer.Array, buffer.Offset, result.Count);
                    }
                    while (!result.EndOfMessage);

                    if (result.MessageType == WebSocketMessageType.Close)
                    {
                        await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
                        break; // Exit loop on close
                    }

                    ms.Seek(0, System.IO.SeekOrigin.Begin);
                    using (var reader = new System.IO.StreamReader(ms, Encoding.UTF8))
                    {
                        var message = await reader.ReadToEndAsync();

                        try
                        {
                            var json = JObject.Parse(message);

                            if (json["event"]?["source"]?.ToString() == "YouTube" && json["event"]?["type"]?.ToString() == "Message")
                            {
                                var username = json["data"]?["user"]?["name"]?.ToString();
                                var text = json["data"]?["message"]?.ToString().Trim();

                                if (!string.IsNullOrEmpty(username) && !string.IsNullOrEmpty(text))
                                {
                                    OnMessage?.Invoke(this, new OnMessageArgs { ClientId = username, Username = username, Message = text });
                                }
                                else
                                {
                                    m_Logger.Warning($"Extracted username or text was null or empty. Username: '{username}', Text: '{text}'");
                                }
                            }
                        }
                        catch (Newtonsoft.Json.JsonReaderException jex)
                        {
                            m_Logger.Error(jex, $"Failed to parse JSON message from Streamer.bot. Raw message: '{message}'");
                        }
                        catch (Exception ex)
                        {
                            m_Logger.Error(ex, "An unexpected error occurred while processing message from Streamer.bot.");
                        }
                    }
                }
            }
        }
    }
}
