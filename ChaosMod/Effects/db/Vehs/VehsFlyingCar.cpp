/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, 0))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(player, 0);
		int vehClass = GET_VEHICLE_CLASS(veh);
		if (vehClass == 15 || vehClass == 16)
		{
			return;
		}
		float currentSpeed = GET_ENTITY_SPEED(veh);
		float maxSpeed = GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(GET_ENTITY_MODEL(veh));
		if (currentSpeed < maxSpeed * 0.6)
		{
			return;
		}
		if (IS_CONTROL_PRESSED(0, 71)) //Forward
		{
			SET_VEHICLE_FORWARD_SPEED(veh, min(maxSpeed * 3, currentSpeed + 0.3));
		}

		if (vehClass == 14 || !IS_VEHICLE_ON_ALL_WHEELS(veh)) // Allow Steering if not "on ground" or boat in water
		{
			if (IS_CONTROL_PRESSED(0, 63)) //Turn Left
			{
				Vector3 Rot = GET_ENTITY_ROTATION(veh, 2);
				Rot.z += 1.0;
				SET_ENTITY_ROTATION(veh, Rot.x, Rot.y, Rot.z, 2, 1);
			}

			if (IS_CONTROL_PRESSED(0, 64)) //Turn Right
			{
				Vector3 Rot = GET_ENTITY_ROTATION(veh, 2);
				Rot.z -= 1.0;
				SET_ENTITY_ROTATION(veh, Rot.x, Rot.y, Rot.z, 2, 1);
			}

			if (IS_CONTROL_PRESSED(0, 108)) //Roll Left
			{
				Vector3 Rot = GET_ENTITY_ROTATION(veh, 2);
				Rot.y -= 1.0;
				SET_ENTITY_ROTATION(veh, Rot.x, Rot.y, Rot.z, 2, 1);
			}

			if (IS_CONTROL_PRESSED(0, 109)) //Roll Right
			{
				Vector3 Rot = GET_ENTITY_ROTATION(veh, 2);
				Rot.y += 1.0;
				SET_ENTITY_ROTATION(veh, Rot.x, Rot.y, Rot.z, 2, 1);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_FLYING_CAR, nullptr, nullptr, OnTick);