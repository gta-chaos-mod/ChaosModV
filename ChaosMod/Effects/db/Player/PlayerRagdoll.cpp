#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_RAGDOLL, OnStart);