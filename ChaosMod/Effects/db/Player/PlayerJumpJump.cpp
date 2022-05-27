/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Memory/Physics.h"

static int lastCheck = 0;

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	if (lastCheck + 500 < GET_GAME_TIMER() && !IS_PED_JUMPING(player))
	{
		if (IS_PED_IN_ANY_VEHICLE(player, false))
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);
			if (!IS_ENTITY_IN_AIR(veh))
			{
				Memory::ApplyForceToEntityCenterOfMass(veh, 0, .0f, .0f, 200.f, true, false, true, true);
				lastCheck = GET_GAME_TIMER();
			}
		}
		else
		{
			TASK_CLIMB(player, false);
			lastCheck = GET_GAME_TIMER();
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Jump! Jump!",
		.Id = "player_jump_jump",
		.IsTimed = true,
		.IsShortDuration = true
	}
);