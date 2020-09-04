/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	static int lastTick = GET_GAME_TIMER();
	static bool doBurst = true;
	int currentTick = GET_GAME_TIMER();

	if (lastTick < currentTick)
	{

		for (Vehicle veh : GetAllVehs())
		{
			for (int i = 0; i < 48; i++) // using code from VehsPopTires.cpp
			{
				if (doBurst)
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
		if (doBurst)
		{
			lastTick = currentTick + 750;
		}
		else
		{
			lastTick = currentTick + g_random.GetRandomInt(1000, 5000);
		}
		doBurst = !doBurst;
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_DRIVE_OR_NOT, nullptr, nullptr, OnTick);
