/*
    Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 currentTick     = GET_GAME_TIMER();

	if (lastTick < currentTick - 500) // every second, half of second
	{
		for (auto veh : GetAllVehs())
		{
			for (int i = 0; i < 7; i++)
			{
				SET_VEHICLE_DOOR_OPEN(veh, i, false, false);
				_SET_VEHICLE_DOOR_CAN_BREAK(veh, i, false);
			}
		}
	}

	if (lastTick < currentTick - 1000) // every second, end of
	{
		lastTick = currentTick;
		for (auto veh : GetAllVehs())
		{
			SET_VEHICLE_DOORS_SHUT(veh, false); // Closes ALL doors of vehicle
		}
	}
}

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		for (int i = 0; i < 7; i++)
		{
			_SET_VEHICLE_DOOR_CAN_BREAK(veh, i, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Spammy Vehicle Doors",
		.Id = "vehs_spamdoors",
		.IsTimed = true
	}
);