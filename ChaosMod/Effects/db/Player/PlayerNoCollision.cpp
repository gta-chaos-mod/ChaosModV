/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

Vehicle lastVeh;

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);
	Entity ent = player;
	if (DOES_ENTITY_EXIST(veh)) ent = veh;
	for (Vehicle e : GetAllVehs())
	{
		SET_ENTITY_NO_COLLISION_ENTITY(e, ent, true);
	}
	for (Ped e : GetAllPeds())
	{
		SET_ENTITY_NO_COLLISION_ENTITY(e, ent, true);
	}
	for (Object e : GetAllProps())
	{
		SET_ENTITY_NO_COLLISION_ENTITY(e, ent, true);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_NO_COLLISION, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Player Has No Collision",
		.Id = "player_no_collide",
		.IsTimed = true,
		.IsShortDuration = true
	}
);