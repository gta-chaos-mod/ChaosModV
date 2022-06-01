/*
    Effect by Juhana
*/

#include <stdafx.h>

static void OnTick()
{
	std::vector<Vehicle> vehs = GetAllVehsArray();
	int vehicleAmount         = static_cast<int>(vehs.size());

	if (vehicleAmount > 0)
	{
		// apply random damage to a random part of a random vehicle
		Vehicle veh = vehs[g_Random.GetRandomInt(0, vehicleAmount - 1)];
		SET_VEHICLE_DAMAGE(veh, g_Random.GetRandomFloat(-1.f, 1.f), g_Random.GetRandomFloat(-1.f, 1.f),
		                   g_Random.GetRandomFloat(-1.f, 1.f), g_Random.GetRandomFloat(1000.f, 10000.f),
		                   g_Random.GetRandomFloat(100.f, 1000.f), true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Crumbling Vehicles",
		.Id = "vehs_crumble",
		.IsTimed = true,
		.IsShortDuration = true
	}
);