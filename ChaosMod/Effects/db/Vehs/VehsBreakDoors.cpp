#include <stdafx.h>

static void OnStart()
{
	int count = 10;

	for (Vehicle veh : GetAllVehs())
	{
		for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
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

			if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
			{
				// Repair doors so they can detach again
				SET_VEHICLE_FIXED(veh);
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