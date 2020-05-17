#include <stdafx.h>

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

			TASK_COMBAT_PED(ped, playerPed, 0, 16);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_ATTACK_PLAYER, nullptr, nullptr, OnTick);