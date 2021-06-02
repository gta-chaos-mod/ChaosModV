/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 currentTick = GET_GAME_TIMER();

	if (lastTick < currentTick - 1750) // 1750MS = every 1.75 seconds.
	{
		lastTick = currentTick;

		for (Vehicle veh : GetAllVehs())
		{
			for (int i = 0; i < 48; i++) // using code from VehsPopTires.cpp
			{
				if (g_Random.GetRandomInt(0, 1)) // random true / false to get ideally 50% of tires popped.
				{
					SET_VEHICLE_TYRES_CAN_BURST(veh, true);
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

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRE_LOOP, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Random Tire Popping",
		.Id = "veh_poptire",
		.IsTimed = true,
		.IsShortDuration = true
	}
);