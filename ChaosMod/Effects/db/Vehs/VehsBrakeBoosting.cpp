#include <stdafx.h>

#include "Memory/Physics.h"
#include "Memory/Vehicle.h"

static void OnTick()
{
	static const Hash blimpHash = GET_HASH_KEY("BLIMP");

	for (Vehicle veh : GetAllVehs())
	{
		Hash vehModel = GET_ENTITY_MODEL(veh);
		int vehClass  = GET_VEHICLE_CLASS(veh);

		// Exclude helis since the "braking" flag seems to be always set for those
		// Also manually exclude blimps since those don't seem to be categorized as either of those
		if (vehClass != 15 && vehModel != blimpHash && Memory::IsVehicleBraking(veh))
		{
			Memory::ApplyForceToEntity(veh, 0, .0f, 50.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Brake Boosting",
		.Id = "veh_brakeboost",
		.IsTimed = true
	}
);