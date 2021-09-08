#include <stdafx.h>

static void OnStart()
{
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		for (Vehicle veh : GetAllVehs())
		{
			for (int j = 0; j < 48; j++)
			{
				SET_VEHICLE_TYRES_CAN_BURST(veh, true);
				SET_VEHICLE_TYRE_BURST(veh, j, true, 1000.f);
			}
		}

		if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
		{
			// Wait, fix tyres, and pop them again
			WAIT(300);

			for (Vehicle veh : GetAllVehs())
			{
				for (int j = 0; j < 48; j++)
				{
					SET_VEHICLE_TYRE_FIXED(veh, j);
				}
			}

			WAIT(300);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRES, OnStart, EffectInfo
	{
		.Name = "Pop Tires Of Every Vehicle",
		.Id = "playerveh_poptires"
	}
);