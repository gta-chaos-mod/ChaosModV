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
                    m_Logger.Debug($"Received raw message from Streamer.bot: {message}");

                    try
                    {
                        m_Logger.Debug("Attempting to parse message as JObject.");
                        var json = JObject.Parse(message);
                        m_Logger.Debug($"Successfully parsed message. JSON: {json.ToString(Newtonsoft.Json.Formatting.None)}");

                        if (json["event"]?["source"]?.ToString() == "YouTube" && json["event"]?["type"]?.ToString() == "Message")
                        {
                            m_Logger.Debug("Message is a YouTube Message event.");
                            var username = json["data"]?["user"]?["displayName"]?.ToString();
                            var text = json["data"]?["message"]?.ToString().Trim();

                            m_Logger.Debug($"Extracted username: '{username}', text: '{text}'");

                            if (!string.IsNullOrEmpty(username) && !string.IsNullOrEmpty(text))
                            {
                                m_Logger.Information($"Valid YouTube message received. User: '{username}', Message: '{text}'. Invoking OnMessage event.");
                                OnMessage?.Invoke(this, new OnMessageArgs
                                {
                                    ClientId = username, // Using username as client id
                                    Username = username,
                                    Message = text
                                });
                                m_Logger.Information("OnMessage event invoked.");
                            }
                            else
                            {
                                m_Logger.Warning($"Extracted username or text was null or empty. Username: '{username}', Text: '{text}'");
                            }
                        }
                        else
                        {
                            m_Logger.Debug($"Message is not a YouTube Message event. Event source: '{json["event"]?["source"]}', type: '{json["event"]?["type"]}'");
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
