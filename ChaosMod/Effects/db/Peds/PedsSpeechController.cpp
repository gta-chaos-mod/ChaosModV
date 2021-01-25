#include <stdafx.h>

static void OnTickFriendly()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char* speechesFriendly[3]{ "GENERIC_HI", "GENERIC_HOWS_IT_GOING", "GENERIC_THANKS" };

				int choice = g_random.GetRandomInt(0, 2);
				_PLAY_AMBIENT_SPEECH1(ped, speechesFriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

static RegisterEffect registerEffect1(EFFECT_PEDS_SAY_HI, nullptr, nullptr, OnTickFriendly);

static void OnTickUnfriendly()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char* speechesUnfriendly[3]{ "GENERIC_CURSE_HIGH", "GENERIC_INSULT_HIGH", "GENERIC_WAR_CRY" };

				int choice = g_random.GetRandomInt(0, 2);
				_PLAY_AMBIENT_SPEECH1(ped, speechesUnfriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

static RegisterEffect registerEffect2(EFFECT_PEDS_INSULT, nullptr, nullptr, OnTickUnfriendly);

static void OnTickKifflom()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				_PLAY_AMBIENT_SPEECH1(ped, "KIFFLOM_GREET", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

static RegisterEffect registerEffect3(EFFECT_PEDS_KIFFLOM, nullptr, nullptr, OnTickKifflom);