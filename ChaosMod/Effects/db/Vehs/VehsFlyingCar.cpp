/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, 0))
	{
		Vehicle veh  = GET_VEHICLE_PED_IS_IN(player, 0);
		int vehClass = GET_VEHICLE_CLASS(veh);
		if (vehClass == 15 || vehClass == 16)
		{
			return;
		}
		float currentSpeed = GET_ENTITY_SPEED(veh);
		float maxSpeed     = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh));
		if (currentSpeed < maxSpeed * 0.6)
		{
			return;
		}
		if (IS_CONTROL_PRESSED(0, 71)) // Forward
		{
			SET_VEHICLE_FORWARD_SPEED(veh, std::min(maxSpeed * 3.f, currentSpeed + 0.3f));
		}

		if (vehClass == 14 || !IS_VEHICLE_ON_ALL_WHEELS(veh)) // Allow Steering if not "on ground" or boat in water
		{
			Vector3 rot = GET_ENTITY_ROTATION(veh, 2);
			if (IS_CONTROL_PRESSED(0, 63)) // Turn Left
			{
				rot.z += 1.0;
			}
			if (IS_CONTROL_PRESSED(0, 64)) // Turn Right
			{
				rot.z -= 1.0;
			}

			if (IS_CONTROL_PRESSED(0, 108)) // Roll Left
			{
				rot.y -= 1.0;
			}

			if (IS_CONTROL_PRESSED(0, 109)) // Roll Right
			{
				rot.y += 1.0;
			}

			if (IS_CONTROL_PRESSED(0, 111)) // Tilt Down
			{
				rot.x -= 1.0;
			}

			if (IS_CONTROL_PRESSED(0, 112)) // Tilt Up
			{
				rot.x += 1.0;
			}
			SET_ENTITY_ROTATION(veh, rot.x, rot.y, rot.z, 2, 1);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Flying Cars",
		.Id = "vehs_flyingcars",
		.IsTimed = true
	}
);