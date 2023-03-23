/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static float GetRandomForce(bool negativeAllowed)
{
	return g_Random.GetRandomFloat(50, 100) * (negativeAllowed && g_Random.GetRandomInt(0, 2) == 0 ? -1 : 1);
}

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
		SET_PED_TO_RAGDOLL(player, 5000, 0, 0, true, true, false);
	}
	Memory::ApplyForceToEntityCenterOfMass(entityToFlip, 1, GetRandomForce(true), GetRandomForce(true),
	                                       GetRandomForce(false), false, false, true, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Fling Player",
        .Id = "player_fling_player"
    }
);