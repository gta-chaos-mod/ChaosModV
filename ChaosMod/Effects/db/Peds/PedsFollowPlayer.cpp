#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, playerPed, .0f, .0f, .0f, 9999.f, -1, .0f, true);
			SET_PED_KEEP_TASK(ped, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_FOLLOW_PLAYER, OnStart);