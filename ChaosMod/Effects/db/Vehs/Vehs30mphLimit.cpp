#include <stdafx.h>

static void OnTick()
{
	auto ped = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(ped, false))
	{
		SET_ENTITY_MAX_SPEED(GET_VEHICLE_PED_IS_IN(ped, false), 13.41); // 13.41 Meters Per Second = 30 MPH~
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_TOPSPEED_30MPH, nullptr, nullptr, OnTick);