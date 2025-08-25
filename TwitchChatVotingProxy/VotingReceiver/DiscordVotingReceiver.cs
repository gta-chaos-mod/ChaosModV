﻿using Discord;
using Discord.Net;
using Discord.WebSocket;
using Serilog;
using Shared;
using TwitchChatVotingProxy.ChaosPipe;

namespace TwitchChatVotingProxy.VotingReceiver
{
    /// <summary>
    /// Discord voting receiver implementation
    /// </summary>
    class DiscordVotingReceiver : IVotingReceiver
    {
        public event EventHandler<OnMessageArgs>? OnMessage = null;

        private readonly string? m_BotToken = null;
        private readonly ulong? m_GuildId = null;
        private readonly ulong? m_ChannelId = null;

        private DiscordSocketClient? m_Client = null;
        private readonly ChaosPipeClient m_ChaosPipe;
        private readonly ILogger m_Logger = Log.Logger.ForContext<DiscordVotingReceiver>();

        private bool m_IsReady = false;

        public DiscordVotingReceiver(OptionsFile config, ChaosPipeClient chaosPipe)
        {
            m_BotToken = config.ReadValue<string>("DiscordBotToken");
            m_GuildId = config.ReadValue<ulong>("DiscordGuildId", 0);
            m_ChannelId = config.ReadValue<ulong>("DiscordChannelId", 0);

            m_ChaosPipe = chaosPipe;
        }

        public async Task<bool> Init()
        {
            if (string.IsNullOrWhiteSpace(m_BotToken))
            {
                m_Logger.Fatal("Discord bot token is not set!");
                m_ChaosPipe.SendErrorMessage("Discord bot token is not set. Please verify your config.");

                return false;
            }

            m_Client = new DiscordSocketClient(new DiscordSocketConfig()
            {
                // We don't actually need any but the Ready event fires much later without this for whatever reason
                // So we just enable all unprivileged intents minus the ones the log warns us against using
                GatewayIntents = GatewayIntents.AllUnprivileged
                    & ~(GatewayIntents.GuildScheduledEvents | GatewayIntents.GuildInvites)
            });

            m_Client.Log += OnLog;
            m_Client.Ready += OnReady;
            m_Client.Disconnected += OnDisconnected;
            m_Client.SlashCommandExecuted += OnSlashCommandExecuted;
            m_Client.ButtonExecuted  += OnButtonExecuted;

            await m_Client.LoginAsync(TokenType.Bot, m_BotToken);
            await m_Client.StartAsync();

            while (!m_IsReady)
                await Task.Delay(100);

            return true;
        }

        public async Task SendMessage(string message)
        {
            if (m_Client is null || m_GuildId is null || m_ChannelId is null)
                return;

            void handleFatal()
            {
                m_Logger.Fatal("Error while sending discord message to channel.");
                m_ChaosPipe.SendErrorMessage("Could not send message to Discord channel." +
                    " Please verify if the server and channel IDs are correct and the bot has permissions to post to that channel.");
            }

            var guild = m_Client.GetGuild(m_GuildId.Value);
            if (guild == null || !guild.IsConnected)
            {
                handleFatal();
                return;
            }

            var channel = guild.GetTextChannel(m_ChannelId.Value);
            if (channel == null)
            {
                handleFatal();
                return;
            }

            try
            {
                var builder = new ComponentBuilder()
                    .WithButton("1", "1", row: 0)
                    .WithButton("2", "2", row: 0)
                    .WithButton("3", "3", row: 0)
                    .WithButton("4", "4", row: 0)
                    .WithButton("5", "5", row: 1)
                    .WithButton("6", "6", row: 1)
                    .WithButton("7", "7", row: 1)
                    .WithButton("8", "8", row: 1);
                await channel.SendMessageAsync(message, components: builder.Build());
            }
            catch (HttpException)
            {
                handleFatal();
                return;
            }
        }

        /// <summary>
        /// Called when the discord client wants to log (callback)
        /// </summary>
        private Task OnLog(LogMessage log)
        {
            switch (log.Severity)
            {
            case LogSeverity.Critical:
                m_Logger.Fatal($"{log.Message}");
                break;
            case LogSeverity.Error:
                m_Logger.Error($"{log.Message}");
                break;
            case LogSeverity.Warning:
                m_Logger.Warning($"{log.Message}");
                break;
            case LogSeverity.Info:
                m_Logger.Information($"{log.Message}");
                break;
            }

            return Task.CompletedTask;
        }
        /// <summary>
        /// Called when the discord client is ready (callback)
        /// </summary>
        private async Task OnReady()
        {
            if (m_Client is null)
                return;

            m_Logger.Information("Successfully connected to discord");

            async void registerCommand(string name)
            {
                await m_Client.CreateGlobalApplicationCommandAsync(new SlashCommandBuilder()
                {
                    Name = name,
                    Description = "Vote for one of the options in Chaos Mod"
                }
                .AddOption("option", ApplicationCommandOptionType.String, "Option to vote for", isRequired: true)
                .Build());
            }

            registerCommand("vote");
            registerCommand("v");

            m_IsReady = true;

            await Task.FromResult(0);
        }
        /// <summary>
        /// Called when the discord client disconnects (callback)
        /// </summary>
        private Task OnDisconnected(Exception exception)
        {
            m_Logger.Information($"Discord client disconnected: {exception}");

            if (exception is HttpException && exception is HttpException { HttpCode: System.Net.HttpStatusCode.Unauthorized })
                m_ChaosPipe.SendErrorMessage("Discord bot token is invalid. Please verify your config.");

            return Task.CompletedTask;
        }
        /// <summary>
        /// Called when the discord client receives a slash command
        /// </summary>
        public async Task OnSlashCommandExecuted(SocketSlashCommand command)
        {
            string option = ((string)command.Data.Options.FirstOrDefault()).Trim();

            if (string.IsNullOrEmpty(option))
            {
                await command.RespondAsync("Missing option", ephemeral: true);
                return;
            }

            await command.RespondAsync($"Voted for {option}", ephemeral: true);

            OnMessage?.Invoke(this, new OnMessageArgs()
            {
                Message = option,
                ClientId = $"{command.User.Id}",
                Username = command.User.GlobalName
            });
        }
        /// <summary>
        /// Called when the discord client receives a button press
        /// </summary>
        public async Task OnButtonExecuted(SocketMessageComponent component)
        {
            string option = component.Data.CustomId;
            
            // pretty sure this would be useless for buttons, I only followed the exact code to not make a mistake
            if (string.IsNullOrEmpty(option))
            {
                await component.RespondAsync("Missing option", ephemeral: true);
                return;
            }

            await component.RespondAsync($"Voted for {option}", ephemeral: true);

            OnMessage?.Invoke(this, new OnMessageArgs()
            {
                Message = option,
                ClientId = $"{component.User.Id}",
                Username = component.User.GlobalName
            });
        }
    }
}
