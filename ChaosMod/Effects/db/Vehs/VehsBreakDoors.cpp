#include <stdafx.h>

static void OnStart()
{
	int count = 10;

	for (Vehicle veh : GetAllVehs())
	{
		for (int i = 0; i < 6; i++)
		{
			SET_VEHICLE_DOOR_BROKEN(veh, i, false);

			if (--count == 0)
			{
				count = 10;

				WAIT(0);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_BREAK_VEH_DOORS, OnStart, EffectInfo
	{
		.Name = "Break Doors Of Every Vehicle",
		.Id = "playerveh_breakdoors"
	}
);