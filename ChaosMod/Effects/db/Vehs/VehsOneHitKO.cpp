/*
	Effect by Lucas7yoshi
*/

#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 curTick = GetTickCount64();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Vehicle veh : GetAllVehs())
		{
			Hash vehModel = GET_ENTITY_MODEL(veh);

			if (!IS_THIS_MODEL_A_HELI(vehModel))
			{
				SET_VEHICLE_OUT_OF_CONTROL(veh, false, true);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_OHKO_VEHICLES, nullptr, nullptr, OnTick);