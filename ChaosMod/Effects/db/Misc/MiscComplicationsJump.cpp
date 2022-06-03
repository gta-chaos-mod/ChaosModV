/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

#include "Memory/Physics.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStart()
{
	Hooks::EnableScriptThreadBlock();
	Ped player  = PLAYER_PED_ID();

	Vehicle veh = CreatePoolVehicle(GET_HASH_KEY("TAILGATER"), -750.44f, 66.58f, 54.58f, 23.29f); //53.28f
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	SET_ENTITY_ROTATION(veh, 25.f, 0.f, 23.29f, 2, true);

	//Create the gardener truck
	Vehicle tempVeh = CreatePoolVehicle(GET_HASH_KEY("BISON3"), -801.25f, 164.60f, 71.07f, 110.f);
	SET_VEHICLE_MOD_COLOR_1(tempVeh, 0, 111, 0);
	
	SET_PED_INTO_VEHICLE(player, veh, -1);
	SET_ENTITY_INVINCIBLE(player, true);

	Memory::ApplyForceToEntityCenterOfMass(veh, 1, 0.f, 100.f, 0.f, false, true, true, false);

	WAIT(1500);
	TASK_LEAVE_VEHICLE(player, veh, 4160);

	int waited = 0;
	while (!IS_PED_GETTING_UP(player) && waited < 100)
	{
		WAIT(100);
		waited++;
	}

	SET_ENTITY_INVINCIBLE(player, false);
	Hooks::DisableScriptThreadBlock();
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&tempVeh);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Complications",
		.Id = "misc_complications"
	}
);