#include <stdafx.h>

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 currentTick = GetTickCount64();

	if (lastTick < currentTick - 500) //every second, half of second
	{
		for (auto veh : GetAllVehs())
		{
			for (int i = 0; i < 7; i++)
			{
				SET_VEHICLE_DOOR_OPEN(veh, i, false, false);
			}
		}
	}

	if (lastTick < currentTick - 1000) //every second, end of
	{
		lastTick = currentTick;
		for (auto veh : GetAllVehs())
		{
			SET_VEHICLE_DOORS_SHUT(veh, false); // Closes ALL doors of vehicle
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SPAM_DOORS, nullptr, nullptr, OnTick);