/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		Memory::SetVehicleOutOfControl(veh, false);
	}
}

static void OnTick()
{
	static DWORD64 lastTick = MISC::GET_GAME_TIMER();
	DWORD64 curTick = MISC::GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Vehicle veh : GetAllVehs())
		{
			Memory::SetVehicleOutOfControl(veh, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_OHKO_VEHICLES, nullptr, OnStop, OnTick);