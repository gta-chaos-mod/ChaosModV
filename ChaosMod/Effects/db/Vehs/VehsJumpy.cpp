#include <stdafx.h>

#include "Memory/Physics.h"

static void OnTick()
{
	static int lastTick = GET_GAME_TIMER();
	int curTick         = GET_GAME_TIMER();

	Vehicle playerVeh   = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);
	if (lastTick < curTick - 50)
	{
		lastTick = curTick;

		for (Vehicle veh : GetAllVehs())
		{
			if (veh != playerVeh && !IS_ENTITY_IN_AIR(veh))
			{
				Memory::ApplyForceToEntityCenterOfMass(veh, 0, .0f, .0f, 500.f, true, false, true, true);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Jumpy Vehicles",
		.Id = "vehs_jumpy",
		.IsTimed = true,
		.IsShortDuration = true
	}
);