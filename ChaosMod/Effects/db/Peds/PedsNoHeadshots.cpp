#include <stdafx.h>

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SUFFERS_CRITICAL_HITS(ped, true);
		}
	}
}

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_NO_HEADSHOTS, nullptr, OnStop, OnTick);