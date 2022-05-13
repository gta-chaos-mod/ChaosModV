/*
	Effect By OnlyRealNubs, Last0xygen
*/

#include "stdafx.h"

static const float force = 13.f;
static const float offsetX = 0.3;

static void OnTick()
{
	static int count = 5;

	for (Vehicle veh : GetAllVehs())
	{
		bool available = IS_VEHICLE_SEAT_FREE(veh, -1, false) ? true : !IS_PED_A_PLAYER(GET_PED_IN_VEHICLE_SEAT(veh, -1, false));

		if (available)
		{
			Memory::ApplyForceToEntity(veh, 3, 0.f, 0.f, force, -offsetX, 0.f, 0.f, 0, true, true, true, false, true);
			Memory::ApplyForceToEntity(veh, 3, 0.f, 0.f, -force, offsetX, 0.f, 0.f, 0, true, true, true, false, true);
			SET_ENTITY_INVINCIBLE(veh, true);
		}

		if (--count == 0)
		{
			count = 5;

			WAIT(0);
		}
	}
}

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
		SET_ENTITY_INVINCIBLE(veh, false);
}

static RegisterEffect registerEffect(EFFECT_VEHS_ROLLING, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Rolling Stones",
		.Id = "vehs_rolling",
		.IsTimed = true,
		.IsShortDuration = true
	}
);