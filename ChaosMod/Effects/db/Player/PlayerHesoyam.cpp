#include <stdafx.h>

static void OnStart()
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
}

static RegisterEffect registerEffect(EFFECT_HEAL, OnStart);