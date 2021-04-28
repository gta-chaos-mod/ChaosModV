#include <stdafx.h>

static const char* APPLICATION_ID = "805142647458103388";
static int64_t StartTime;
static const int SendPresence = 1;

class Discord
{
public:
	static void updateDiscordPresence(std::string CurrentEffect, std::string PreviousEffect);
	static void handleDiscordReady(const DiscordUser* connectedUser);
	static void discordInit();
	static void gameLoop(std::string CurrentEffect, std::string PreviousEffect);
};