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

		Ped playerPed = PLAYER_PED_ID();

		for (Vehicle veh : GetAllVehs())
		{
			if (!IS_PED_IN_ANY_HELI(playerPed))
			{
				SET_VEHICLE_OUT_OF_CONTROL(veh, false, true);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_OHKO_VEHICLES, nullptr, nullptr, OnTick);