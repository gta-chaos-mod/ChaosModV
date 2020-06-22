#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_RAGDOLL, OnStart);