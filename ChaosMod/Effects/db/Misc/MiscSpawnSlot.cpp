/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static void OnStart()
{
	unsigned int slotModel = 0x8CD7617F;	
	REQUEST_MODEL(slotModel);
	while (!HAS_MODEL_LOADED(slotModel))
	{
		WAIT(0);
	}
	Ped playerPed = PLAYER_PED_ID();
	Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 3, 0.3);
	Vector3 rot = { 0, 0, -GET_ENTITY_HEADING(playerPed) };
	int obj = CREATE_OBJECT(slotModel, pos.x, pos.y, pos.z, 1, 1, 1);
	SET_ENTITY_ROTATION(obj, rot.x, rot.y, rot.z, 2, 1);
}

static RegisterEffect registerEffect(EFFECT_MISC_SPAWN_SLOT, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Casino Game",
		.Id = "misc_spawn_slot"
	}
);