#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		for (int i = 0; i < 48; i++)
		{
			SET_VEHICLE_TYRES_CAN_BURST(veh, true);
			SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.f);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRES, OnStart, EffectInfo
	{
		.Name = "Pop Tires Of Every Vehicle",
		.Id = "playerveh_poptires"
	}
);