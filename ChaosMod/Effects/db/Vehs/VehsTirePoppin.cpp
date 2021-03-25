#include <stdafx.h>

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		for (int i = 0; i < 47; i++)
		{
			SET_VEHICLE_TYRE_FIXED(veh, i);
		}
	}
}

static void OnTick()
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

static RegisterEffect registerEffect(EFFECT_ALL_VEH_POP_TIRES, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Now This Is Some Tire Poppin'",
		.Id = "vehs_poptiresconstant",
		.IsTimed = true,
		.IsShortDuration = true
	}
);