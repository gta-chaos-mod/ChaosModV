#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTickFriendly()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char *speechesFriendly[3] { "GENERIC_HI", "GENERIC_HOWS_IT_GOING", "GENERIC_THANKS" };

				int choice = g_Random.GetRandomInt(0, 2);
				PLAY_PED_AMBIENT_SPEECH_NATIVE(ped, speechesFriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickFriendly, 
	{
		.Name = "Friendly Neighborhood",
		.Id = "peds_sayhi",
		.IsTimed = true
	}
);
// clang-format on

static void OnTickUnfriendly()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char *speechesUnfriendly[3] { "GENERIC_CURSE_HIGH", "GENERIC_INSULT_HIGH",
					                                          "GENERIC_WAR_CRY" };

				int choice = g_Random.GetRandomInt(0, 2);
				PLAY_PED_AMBIENT_SPEECH_NATIVE(ped, speechesUnfriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickUnfriendly, 
	{
		.Name = "Unfriendly Neighborhood",
		.Id = "peds_insult",
		.IsTimed = true
	}
);
// clang-format on

static void OnTickKifflom()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
				PLAY_PED_AMBIENT_SPEECH_NATIVE(ped, "KIFFLOM_GREET", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickKifflom, 
	{
		.Name = "Kifflom!",
		.Id = "peds_kifflom",
		.IsTimed = true
	}
);