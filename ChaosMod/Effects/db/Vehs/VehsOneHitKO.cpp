/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

#include "Memory/Vehicle.h"

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		Memory::SetVehicleOutOfControl(veh, false);
	}
}

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Vehicle veh : GetAllVehs())
		{
			Memory::SetVehicleOutOfControl(veh, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vehicles Explode On Impact",
		.Id = "vehs_ohko",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_invincible" }
	}
);