/*
    Effect by ProfessorBiddle, modified
*/

#include <stdafx.h>

#include "Memory/Physics.h"

#include "Util/HelpText.h"

static void OnTick()
{
	auto playerPed = PLAYER_PED_ID();
	auto veh       = GET_VEHICLE_PED_IS_IN(playerPed, false);
	if (!veh || !IS_VEHICLE_ON_ALL_WHEELS(veh))
	{
		return;
	}

	DisplayHelpText("Press ~INPUT_JUMP~ to do a sick ramp jump in your vehicle.");

	if (IS_CONTROL_JUST_PRESSED(0, 22))
	{
		APPLY_FORCE_TO_ENTITY(veh, 0, .0f, 850.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);

		auto playerPos = GET_ENTITY_COORDS(playerPed, false);
		auto rampPos   = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 4, 0);

		auto ramp      = CREATE_OBJECT("prop_mp_ramp_02"_hash, rampPos.x, rampPos.y, rampPos.z, true, false, true);
		PLACE_OBJECT_ON_GROUND_PROPERLY(ramp);
		rampPos = GET_ENTITY_COORDS(ramp, false);
		SET_ENTITY_COORDS(ramp, rampPos.x, rampPos.y, rampPos.z - 0.3, true, true, true, false);
		SET_ENTITY_ROTATION(ramp, GET_ENTITY_PITCH(playerPed), -GET_ENTITY_ROLL(playerPed),
		                    GET_ENTITY_HEADING(playerPed), 0, true);
		SET_ENTITY_AS_NO_LONGER_NEEDED(&ramp);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Ramp Jam",
		.Id = "misc_rampjam",
		.IsTimed = true
	}
);