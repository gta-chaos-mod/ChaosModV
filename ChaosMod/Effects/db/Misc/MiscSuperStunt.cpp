/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	Ped player        = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(player, false);
	Vector3 rampPos   = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, 5, 0);

	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
	if (!playerVeh)
	{
		playerVeh = CreateTempVehicleOnPlayerPos("adder"_hash, GET_ENTITY_HEADING(player));
		SET_PED_INTO_VEHICLE(player, playerVeh, -1);
	}

	Object ramp = CreatePoolProp("prop_mp_ramp_03"_hash, rampPos.x, rampPos.y, rampPos.z, false);
	PLACE_OBJECT_ON_GROUND_PROPERLY(ramp);
	rampPos = GET_ENTITY_COORDS(ramp, false);
	SET_ENTITY_COORDS(ramp, rampPos.x, rampPos.y, rampPos.z - 0.3, true, true, true, false);
	SET_ENTITY_ROTATION(ramp, GET_ENTITY_PITCH(playerVeh), -GET_ENTITY_ROLL(playerVeh), GET_ENTITY_HEADING(playerVeh),
	                    0, true);

	// APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(playerVeh, 0, 0.f, 7000.f, 0.f, true, true, true, true);
	Vector3 forward = GET_ENTITY_FORWARD_VECTOR(playerVeh) * 7000.f;
	SET_ENTITY_VELOCITY(playerVeh, forward.x, forward.y, forward.z);

	SET_ENTITY_INVINCIBLE(player, true);
	SET_ENTITY_INVINCIBLE(playerVeh, true);
	WAIT(500);
	SET_ENTITY_INVINCIBLE(player, false);
	SET_ENTITY_INVINCIBLE(playerVeh, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Super Stunt",
		.Id = "misc_superstunt"
	}
);