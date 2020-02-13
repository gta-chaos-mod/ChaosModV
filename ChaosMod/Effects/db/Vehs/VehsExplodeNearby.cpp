#include <stdafx.h>

static void OnStart()
{
	auto playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);

	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			EXPLODE_VEHICLE(veh, true, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_VEHS, OnStart);