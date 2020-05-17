#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		SET_PED_CONFIG_FLAG(ped, 292, false);
	}
}

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 curTick = GetTickCount64();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (auto ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				SET_PED_CONFIG_FLAG(ped, 292, true);
			}
		}
	}

	SET_PED_CONFIG_FLAG(PLAYER_PED_ID(), 292, false);
}

static RegisterEffect registerEffect(EFFECT_PEDS_FROZEN, nullptr, OnStop, OnTick);