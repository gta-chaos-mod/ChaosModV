#include <stdafx.h>

static const char* APPLICATION_ID = "805142647458103388";
static int FrustrationLevel = 0;
static int64_t StartTime;
static int SendPresence = 1;

static void updateDiscordPresence()
{
    if (SendPresence) {
        char CurEffBuffer[256];
        char PrevEffBuffer[256];

        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));

        sprintf_s(CurEffBuffer, "Current Effect: %s", FrustrationLevel);
        discordPresence.details = CurEffBuffer;

        sprintf_s(PrevEffBuffer, "Previous Effect: %s", FrustrationLevel);
        discordPresence.state = PrevEffBuffer;

        discordPresence.startTimestamp = StartTime;
        discordPresence.endTimestamp = time(0) + 5 * 60;
        discordPresence.largeImageKey = "chaoslarge";

        Discord_UpdatePresence(&discordPresence);
    }
    else {
        Discord_ClearPresence();
    }
}

static void handleDiscordReady(const DiscordUser* connectedUser)
{
    LOG("Connected to user " << connectedUser->username << "#" << connectedUser->discriminator << " - " << connectedUser->userId);
}

static void discordInit()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

static void gameLoop()
{
    StartTime = time(0);
    updateDiscordPresence();
#ifdef DISCORD_DISABLE_IO_THREAD
    Discord_UpdateConnection();
#endif
    Discord_RunCallbacks();
}