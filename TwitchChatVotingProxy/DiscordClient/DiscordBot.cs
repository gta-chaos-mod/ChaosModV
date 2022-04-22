using System;
using Serilog;
using System.Collections.Generic;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Discord;
using Discord.Net;
using Discord.WebSocket;
using Discord.API;
using Discord.Interactions;
using Discord.API;
using Discord.Rest;
using VotingProxy;

namespace VotingProxy.VotingChatClient
{
    public class DiscordBot
    {
        private Credentials credentials;
        private DiscordSocketClient _client;
        private ILogger logger = Log.Logger.ForContext<DiscordBot>();
        private SocketTextChannel _channel = null;
        private RestUserMessage curMsg;

        private Dictionary<int, string> buttonIds = new Dictionary<int, string>();

        private ConnectionState connectionState;

        //Events
        public event EventHandler OnBotConnected;

        public event EventHandler OnBotDisconnected;

        public event EventHandler<BotVoteEventArgs> OnVoteReceived;

        //Methods
        private ulong ToSnowflake(string input)
        {
            ulong result = 0;
            try
            {
                result = Convert.ToUInt64(input); //Was originally int32, but if you knew anything about binary you would know that a discord id is way to big to convert to int32. TL;DR used wrong type.
            }
            catch (FormatException)
            {
                
            }
            return result;
        }

        public DiscordBot(Credentials credentials)
        {
            this.credentials = credentials;
        }

        public void SendVoteMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            string title = "Time for a new effect! Vote between:";
            string optMsg = string.Empty;
            string footer = string.Empty;
            foreach (IVoteOption opt in options)
            {
                optMsg += $"{opt.Matches[0]} : {opt.Label} (Votes: 0)\n";
            }
            if (votingMode == EVotingMode.PERCENTAGE)
            {
                footer = "Votes will affect the chance for one of the effects to occur.";
            }

            var embed = new EmbedBuilder
            {
                Title = title,
                Description = optMsg,
            };
            if (!string.IsNullOrEmpty(footer))
            {
                embed.WithFooter(footer);
            }
           
            embed.WithAuthor(_client.CurrentUser);
            embed.WithCurrentTimestamp();
            var comps = new ComponentBuilder();
            buttonIds.Clear();
            foreach (IVoteOption opt in options)
            {
                comps.WithButton(label: $"Effect #{opt.Matches[0]}", customId: $"effect-{opt.Matches[0]}");
                buttonIds.Add(Int32.Parse(opt.Matches[0]), $"effect-{opt.Matches[0]}");
            }

            var msgs = _client.GetGuild(ToSnowflake(credentials.UserId)).GetTextChannel(ToSnowflake(credentials.ChannelId)).GetMessagesAsync().FlattenAsync().Result;
            _client.GetGuild(ToSnowflake(credentials.UserId)).GetTextChannel(ToSnowflake(credentials.ChannelId)).DeleteMessagesAsync(msgs);
            curMsg = _client.GetGuild(ToSnowflake(credentials.UserId)).GetTextChannel(ToSnowflake(credentials.ChannelId)).SendMessageAsync(embed: embed.Build(), components: comps.Build()).Result;
        }

        public void UpdateVoteMessage(List<IVoteOption> options, EVotingMode votingMode)
        {
            string title = "Time for a new effect! Vote between:";
            string optMsg = string.Empty;
            string footer = string.Empty;
            foreach (IVoteOption opt in options)
            {
                optMsg += $"{opt.Matches[0]} : {opt.Label} (Votes: {opt.Votes})\n";
            }
            if (votingMode == EVotingMode.PERCENTAGE)
            {
                footer = "Votes will affect the chance for one of the effects to occur.";
            }

            var embed = new EmbedBuilder
            {
                Title = title,
                Description = optMsg,
            };
            if (!string.IsNullOrEmpty(footer))
            {
                embed.WithFooter(footer);
            }

            embed.WithAuthor(_client.CurrentUser);
            embed.WithCurrentTimestamp();

            curMsg.ModifyAsync(msg => msg.Embed = embed.Build());
        }

        public async Task ButtonHandler(SocketMessageComponent component)
        {
            foreach (var item in buttonIds)
            {
                if (item.Value == component.Data.CustomId)
                {
                    OnVoteReceived?.Invoke(this, new BotVoteEventArgs()
                        {
                            UserId = component.User.Id,
                            Vote = item.Key
                        }
                    );
                    
                    await component.RespondAsync($"You voted for effect #{item.Key}", ephemeral: true);
                    break;
                }
            }
        }

        private Task Hearbeat(int a, int b)
        {
            ConnectionState nowState = _client.ConnectionState;
            if (nowState != connectionState)
            {
                connectionState = nowState;
                switch (connectionState)
                {
                    case ConnectionState.Connected:
                        OnBotConnected?.Invoke(this, EventArgs.Empty);
                        break;
                    case ConnectionState.Disconnected:
                        OnBotDisconnected?.Invoke(this, EventArgs.Empty);
                        break;
                }

            }

            return Task.CompletedTask;
        }

        public void StartBot()
        {
            logger.Information("Starting bot!");
            _client = new DiscordSocketClient();

            _client.LoginAsync(TokenType.Bot, credentials.OAuth);
            _client.StartAsync();
            _client.SetGameAsync("Chaos Mod", type: ActivityType.Playing);

            _client.LatencyUpdated += Hearbeat;
            _client.ButtonExecuted += ButtonHandler;
        }

        public void StopBot()
        {
            logger.Information("Stopping bot!");

            _client.LogoutAsync();
            _client.StopAsync();            
            _client.LatencyUpdated -= Hearbeat;
            _client.ButtonExecuted -= ButtonHandler;
        }
    }

    public class BotVoteEventArgs
    {
        public int Vote;
        public ulong UserId;
    }

}
