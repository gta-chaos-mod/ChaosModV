#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Physics.h"
#include "Memory/Vehicle.h"

static void BoostOnTick()
{
	static const Hash blimpHash = "BLIMP"_hash;

	for (Vehicle veh : GetAllVehs())
	{
		Hash vehModel = GET_ENTITY_MODEL(veh);
		int vehClass  = GET_VEHICLE_CLASS(veh);
		float speed   = GET_ENTITY_SPEED_VECTOR(veh, true).y;

		if (vehClass != 15 && vehModel != blimpHash && Memory::IsVehicleBraking(veh) && speed > 1.f
		    && GET_ENTITY_HEIGHT_ABOVE_GROUND(veh) <= 2.f)
		{
			SET_VEHICLE_FORWARD_SPEED(veh, speed * -1.5f);
		}
	}
}
// clang-format off
REGISTER_EFFECT(nullptr, nullptr, BoostOnTick, 
	{
		.Name = "Boost Braking",
		.Id = "veh_boostbrake",
		.IsTimed = true,
		.IncompatibleWith = { "veh_brakeboost" },
	}
);
