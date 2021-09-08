#include <stdafx.h>

static void OnStart()
{
	int count = 10;

	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (GET_ENTITY_HEALTH(veh) > 0)
			{
				for (int j = 0; j < 6; j++)
				{
					SET_VEHICLE_DOOR_BROKEN(veh, j, false);

					if (--count == 0)
					{
						count = 10;
						WAIT(0);
					}
				}
			}
		}

		if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
		{
			for (Vehicle veh : GetAllVehs())
			{
				// Don't do this for vehicles that are completely destroyed
				if (GET_ENTITY_HEALTH(veh) > 0)
				{
					// Couldn't find a native to repair the doors specifically, so any other damage done to the vehicle will unfortunately also be repaired here
					SET_VEHICLE_FIXED(veh);

					if (--count == 0)
					{
						count = 10;
						WAIT(0);
					}
				}
			}

			WAIT(10);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_BREAK_VEH_DOORS, OnStart, EffectInfo
	{
		.Name = "Break Doors Of Every Vehicle",
		.Id = "playerveh_breakdoors"
	}
);