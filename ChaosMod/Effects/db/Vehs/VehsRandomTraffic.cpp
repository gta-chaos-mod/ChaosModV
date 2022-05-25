// Effect by ProfessorBiddle, this is a bit clunky but it works well.

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		// detect if vehicle already randomized
		auto carModified = GET_VEHICLE_WINDOW_TINT(veh);

		if (carModified != 3)
		{
			Vehicle newVeh = ReplaceVehicle(veh, true);

			// Way to know if vehicle is modified
			SET_VEHICLE_WINDOW_TINT(newVeh, 3);

			Ped driver = GET_PED_IN_VEHICLE_SEAT(newVeh, -1, 0);
			if (driver && DOES_ENTITY_EXIST(driver) && !IS_PED_A_PLAYER(driver))
			{
				TASK_VEHICLE_DRIVE_WANDER(driver, newVeh, 40, 786603);
			}

			WAIT(0);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Random Traffic",
		.Id = "veh_randtraffic",
		.IsTimed = true
	}
);