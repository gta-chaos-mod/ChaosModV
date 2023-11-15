#include <stdafx.h>

static std::unordered_map<Ped, bool> crazyPeds;

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (ped == PLAYER_PED_ID() || !IS_PED_IN_ANY_VEHICLE(ped, false) || crazyPeds[ped])
			continue;

		Vector3 vec = GET_ENTITY_COORDS(ped, false);
		TASK_SMART_FLEE_COORD(ped, vec.x, vec.y, vec.z, 9999.f, -1, false, false);
		// TASK_VEHICLE_DRIVE_WANDER(ped, veh, 100.f, 0x423C0B6C);

		SET_DRIVE_TASK_DRIVING_STYLE(ped, 0x423C0A6C);
		SET_DRIVE_TASK_MAX_CRUISE_SPEED(ped, 200.f);
		SET_DRIVER_ABILITY(ped, 1.f);
		SET_DRIVER_AGGRESSIVENESS(ped, 1.f);

		crazyPeds[ped] = true;
	}
}

static void OnStop()
{
	crazyPeds.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Reckless Drivers",
		.Id = "peds_reckless",
		.IsTimed = true,
		.IncompatibleWith = { "traffic_gtao" }
	}
);