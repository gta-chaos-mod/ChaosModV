#include <stdafx.h>

static void OnTickFriendly()
{
	static auto lastTick = 0;
	auto curTick = GetTickCount64();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (auto ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char* speechesFriendly[3]{ "GENERIC_HI", "GENERIC_HOWS_IT_GOING", "GENERIC_THANKS" };

				int choice = Random::GetRandomInt(0, 2);
				_PLAY_AMBIENT_SPEECH1(ped, speechesFriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

static RegisterEffect registerEffect1(EFFECT_PEDS_SAY_HI, nullptr, nullptr, OnTickFriendly);

static void OnTickUnfriendly()
{
	static auto lastTick = 0;
	auto curTick = GetTickCount64();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (auto ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped))
			{
				constexpr const char* speechesUnfriendly[3]{ "GENERIC_CURSE_HIGH", "GENERIC_INSULT_HIGH", "GENERIC_WAR_CRY" };

				int choice = Random::GetRandomInt(0, 2);
				_PLAY_AMBIENT_SPEECH1(ped, speechesUnfriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}
}

static RegisterEffect registerEffect2(EFFECT_PEDS_INSULT, nullptr, nullptr, OnTickUnfriendly);