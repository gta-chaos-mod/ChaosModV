/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Vehicle.h"

static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();
	auto playerPos = GET_ENTITY_COORDS(playerPed, false);
	auto rampPos   = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.f, 5.f, 0.f);

	auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	if (!IS_PED_IN_VEHICLE(playerPed, playerVeh, true))
	{
		playerVeh = CreateTempVehicleOnPlayerPos("adder"_hash, GET_ENTITY_HEADING(playerPed));
		SET_PED_INTO_VEHICLE(playerPed, playerVeh, -1);
	}

	auto ramp = CreatePoolProp("prop_mp_ramp_03"_hash, rampPos.x, rampPos.y, rampPos.z, false);
	PLACE_OBJECT_ON_GROUND_PROPERLY(ramp);
	rampPos = GET_ENTITY_COORDS(ramp, false);
	SET_ENTITY_COORDS(ramp, rampPos.x, rampPos.y, rampPos.z - 0.3f, true, true, true, false);
	SET_ENTITY_ROTATION(ramp, GET_ENTITY_PITCH(playerVeh), -GET_ENTITY_ROLL(playerVeh), GET_ENTITY_HEADING(playerVeh),
	                    0, true);

	// APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(playerVeh, 0, 0.f, 7000.f, 0.f, true, true, true, true);
	auto forward = GET_ENTITY_FORWARD_VECTOR(playerVeh) * 7000.f;
	SET_ENTITY_VELOCITY(playerVeh, forward.x, forward.y, forward.z);

	SET_ENTITY_INVINCIBLE(playerPed, true);
	SET_ENTITY_INVINCIBLE(playerVeh, true);
	WAIT(500);
	SET_ENTITY_INVINCIBLE(playerPed, false);
	SET_ENTITY_INVINCIBLE(playerVeh, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Super Stunt",
		.Id = "misc_superstunt"
	}
);