/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static void OnStart()
{
	Ped plr = PLAYER_PED_ID();
	Vehicle veh = GET_VEHICLE_PED_IS_IN(plr, false);

	if (!DOES_ENTITY_EXIST(veh))
	{
		Vector3 p = GET_ENTITY_COORDS(plr);
		Vehicle veh = CreatePoolVehicle(GET_HASH_KEY("buffalo"), p.x, p.y, p.z, GET_ENTITY_HEADING(plr));
		SET_PED_INTO_VEHICLE(plr, veh, -1);
	}

	//Roll the vehicle
	APPLY_FORCE_TO_ENTITY(veh, 3, 0.f, 0.f, 13.f, -0.3f, 0.f, 0.f, 0, true, true, true, false, true);
}

static RegisterEffect reg(EFFECT_PLAYER_BARREL_ROLL, OnStart, EffectInfo
	{
		.Name = "Do A Barrel Roll",
		.Id = "player_barrel_roll"
	}
);
