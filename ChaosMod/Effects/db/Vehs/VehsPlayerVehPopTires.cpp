#include <stdafx.h>

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		for (int i = 0; i < 48; i++)
		{
			SET_VEHICLE_TYRE_BURST(GET_VEHICLE_PED_IS_IN(playerPed, false), i, true, 1000.f);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRES, OnStart);