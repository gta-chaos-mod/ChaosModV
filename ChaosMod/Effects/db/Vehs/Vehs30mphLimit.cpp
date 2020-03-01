#include <stdafx.h>

static void OnStart()
{
	// Once the effect gets triggered
}

static void OnStop()
{
	// Once the effect stops
}

static void OnTick()
{
	auto ped = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(ped, false))
	{
		SET_ENTITY_MAX_SPEED(GET_VEHICLE_PED_IS_IN(ped, false), 13.41);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_TOPSPEED_30MPH, OnStart, OnStop, OnTick);