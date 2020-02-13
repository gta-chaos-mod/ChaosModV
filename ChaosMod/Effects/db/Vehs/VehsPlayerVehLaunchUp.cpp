#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_ENTITY_VELOCITY(GET_VEHICLE_PED_IS_IN(playerPed, false), .0f, .0f, 10000.f);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VEH_LAUNCH, OnStart);