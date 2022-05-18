/*
	Effect By OnlyRealNubs, Last0xygen
*/

#include "stdafx.h"
#include "Memory/Physics.h"

static const float force = 20.f;
static const float offsetX = 0.3;

static void OnTick()
{
	static int count = 7;

	for (Vehicle veh : GetAllVehs())
	{
		if (IS_VEHICLE_SEAT_FREE(veh, -1, false) ? true : !IS_PED_A_PLAYER(GET_PED_IN_VEHICLE_SEAT(veh, -1, false)))
		{
			Memory::ApplyForceToEntity(veh, 3, 0.f, 0.f, force, -offsetX, 0.f, 0.f, 0, true, true, true, false, true);
			Memory::ApplyForceToEntity(veh, 3, 0.f, 0.f, -force, offsetX, 0.f, 0.f, 0, true, true, true, false, true);
			SET_ENTITY_INVINCIBLE(veh, true);

			if (GET_ENTITY_SPEED(veh) < 10)
			{
				Vector3 velocity = GET_ENTITY_SPEED_VECTOR(veh, true);
				SET_ENTITY_VELOCITY(veh, velocity.x * 4, velocity.y, velocity.z);
			}
		}

		if (--count == 0)
		{
			count = 7;
			WAIT(0);
		}
	}
}

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
		SET_ENTITY_INVINCIBLE(veh, false);
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Rolling Stones",
		.Id = "vehs_rolling",
		.IsTimed = true,
		.IsShortDuration = true,
	}
);
