#include <stdafx.h>

#include "Util/DiscordRPC.h"

void Discord::updateDiscordPresence(std::string CurrentEffect, std::string PreviousEffect)
{
    if (SendPresence) {
        char CurEffBuffer[256];
        char PrevEffBuffer[256];

        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));

        sprintf_s(CurEffBuffer, sizeof(CurEffBuffer), "Current Effect: %s", CurrentEffect.c_str());
        discordPresence.details = CurEffBuffer;

        sprintf_s(PrevEffBuffer, sizeof(PrevEffBuffer), "Previous Effect: %s", PreviousEffect.c_str());
        discordPresence.state = PrevEffBuffer;

        discordPresence.startTimestamp = StartTime;
        discordPresence.largeImageKey = "chaoslarge";

        Discord_UpdatePresence(&discordPresence);
    }
    else {
        Discord_ClearPresence();
    }
}

void Discord::handleDiscordReady(const DiscordUser* connectedUser)
{
    LOG("Connected to user " << connectedUser->username << "#" << connectedUser->discriminator << " - " << connectedUser->userId);
}

void Discord::discordInit()
{
    StartTime = time(0);
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

void Discord::gameLoop(std::string CurrentEffect, std::string PreviousEffect)
{
    updateDiscordPresence(CurrentEffect, PreviousEffect);
#ifdef DISCORD_DISABLE_IO_THREAD
    Discord_UpdateConnection();
#endif
    Discord_RunCallbacks();
}