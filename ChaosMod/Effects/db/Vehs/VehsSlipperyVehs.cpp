#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_VEHICLE_REDUCE_GRIP(veh, false);
	}
}

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		static bool toggle = true;

		SET_VEHICLE_REDUCE_GRIP(veh, toggle);

		toggle = !toggle;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Slippery Vehicles",
		.Id = "vehs_slippery",
		.IsTimed = true
	}
);