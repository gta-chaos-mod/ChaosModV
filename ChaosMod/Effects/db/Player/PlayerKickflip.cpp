/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Memory/Physics.h"

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
	Memory::ApplyForceToEntity(entityToFlip, 1, 0, 0, 10, 2, 0, 0, 0, true, true, true, false, true);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Kickflip",
		.Id = "player_kickflip"
	}
);