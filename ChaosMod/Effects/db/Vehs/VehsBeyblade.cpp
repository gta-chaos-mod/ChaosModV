/*
	Effect by Last0xygen, modified
*/

#include <stdafx.h>

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
		SET_VEHICLE_REDUCE_GRIP(veh, false);
	}
}

static void OnTick()
{
	int count = 5;

	float force = 100;
	float velocityMultiplier = 3;
	for (Vehicle veh : GetAllVehs())
	{
		bool doBeyblade = IS_VEHICLE_SEAT_FREE(veh, -1, false) ? true : !IS_PED_A_PLAYER(GET_PED_IN_VEHICLE_SEAT(veh, -1, false));

		if (doBeyblade)
		{
			APPLY_FORCE_TO_ENTITY(veh, 3, force, 0, 0, 0, 4, 0, 0, true, true, true, true, true);
			APPLY_FORCE_TO_ENTITY(veh, 3, -force, 0, 0, 0, -4, 0, 0, true, true, true, true, true);
			SET_ENTITY_INVINCIBLE(veh, true);
			SET_VEHICLE_REDUCE_GRIP(veh, true);
			if (GET_ENTITY_SPEED(veh) < 10)
			{
				Vector3 velocity = GET_ENTITY_SPEED_VECTOR(veh, true);
				velocity.x *= velocityMultiplier;
				velocity.y *= velocityMultiplier;
				SET_ENTITY_VELOCITY(veh, velocity.x, velocity.y, velocity.z);
			}
		}
		else
		{
			SET_VEHICLE_REDUCE_GRIP(veh, false);
		}

		if (--count == 0)
		{
			count = 5;

			WAIT(0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_BEYBLADE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Beyblades",
		.Id = "vehs_beyblade",
		.IsTimed = true,
		.IsShortDuration = true
	}
);