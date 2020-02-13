#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			TASK_REACT_AND_FLEE_PED(ped, playerPed);
			SET_PED_KEEP_TASK(ped, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_FLEE, OnStart);