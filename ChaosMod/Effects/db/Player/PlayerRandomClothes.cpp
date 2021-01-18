#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	SET_PED_RANDOM_COMPONENT_VARIATION(playerPed, 0);
	SET_PED_RANDOM_PROPS(playerPed);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_RANDOMCLOTHES, OnStart);