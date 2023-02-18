#include <stdafx.h>

static void OnStart()
{
	int count = 5;

	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_FIXED(veh);
		SET_VEHICLE_DIRT_LEVEL(veh, .0f);
		SET_VEHICLE_ENGINE_HEALTH(veh, 1000.f);
		SET_VEHICLE_PETROL_TANK_HEALTH(veh, 1000.f);
		SET_VEHICLE_BODY_HEALTH(veh, 1000.f);
		SET_VEHICLE_UNDRIVEABLE(veh, false);
		SET_VEHICLE_ON_GROUND_PROPERLY(veh, 5.f);

		if (--count == 0)
		{
			count = 5;

			WAIT(0);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Repair All Vehicles",
		.Id = "playerveh_repair"
	}
);