/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

// Each tire has a 50% chance of popping or being repaired every 2 seconds.
static void OnStart()
{
	//pop tires once via this since OnTick only fires after the loop time
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		for (int i = 0; i < 48; i++) // using code from VehsPopTires.cpp
		{
			if (g_random.GetRandomInt(0, 1)) // random true / false to get ideally 50% of tires popped.
			{
				SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.f);
			}
			else
			{
				SET_VEHICLE_TYRE_FIXED(veh, i);
			}
		}
	}
}
static void OnStop()
{
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		for (int i = 0; i < 48; i++)
		{
			SET_VEHICLE_TYRE_FIXED(veh, i);
		}
	}
}
static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 currentTick = GetTickCount64();

	if (lastTick < currentTick - 2000) // 2000MS = every 2 seconds.
	{
		lastTick = currentTick;

		Ped playerPed = PLAYER_PED_ID();
		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
			for (int i = 0; i < 48; i++) // using code from VehsPopTires.cpp
			{
				if (g_random.GetRandomInt(0, 1)) // random true / false to get ideally 50% of tires popped.
				{
					SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.f);
				}
				else
				{
					SET_VEHICLE_TYRE_FIXED(veh, i);
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRE_LOOP, OnStart, OnStop, OnTick);