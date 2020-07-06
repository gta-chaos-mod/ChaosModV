#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	int maxHealth = GET_PED_MAX_HEALTH(playerPed);

	SET_ENTITY_HEALTH(playerPed, maxHealth, 0);
	ADD_ARMOUR_TO_PED(playerPed, 200);
}

static RegisterEffect registerEffect(EFFECT_HEAL, OnStart);