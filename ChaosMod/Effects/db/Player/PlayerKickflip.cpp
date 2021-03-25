/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	Ped player = PLAYER_PED_ID();
	Entity entityToFlip;
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		entityToFlip = GET_VEHICLE_PED_IS_IN(player, false);
	}
	else
	{
		entityToFlip = player;
		SET_PED_TO_RAGDOLL(player, 200, 0, 0, true, true, false);
	}
	APPLY_FORCE_TO_ENTITY(entityToFlip, 1, 0, 0, 10, 2, 0, 0, 0, true, true, true, false, true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_KICKFLIP, OnStart, EffectInfo
	{
		.Name = "Kickflip",
		.Id = "player_kickflip"
	}
);