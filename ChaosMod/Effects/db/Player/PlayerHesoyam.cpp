#include <stdafx.h>

static void OnStart()
{
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		Ped playerPed = PLAYER_PED_ID();
		int maxHealth = GET_PED_MAX_HEALTH(playerPed);

		SET_ENTITY_HEALTH(playerPed, maxHealth, 0);
		ADD_ARMOUR_TO_PED(playerPed, 200);

		for (int i = 0; i < 3; i++)
		{
			char statNameFull[32];
			sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

			auto hash = GET_HASH_KEY(statNameFull);

			int money;
			STAT_GET_INT(hash, &money, -1);
			STAT_SET_INT(hash, money + 250000, 1);
		}

		// Also repair vehicle
		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

			SET_VEHICLE_FIXED(playerVeh);
		}

		if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
		{
			WAIT(2000);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_HEAL, OnStart, EffectInfo
	{
		.Name = "HESOYAM",
		.Id = "player_heal"
	}
);