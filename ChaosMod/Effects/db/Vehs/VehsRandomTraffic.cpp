#include <stdafx.h>
//effect by ProfessorBiddle, this is a bit clunky but it works well.

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		//detect if vehicle already randomized
		auto carModified = GET_VEHICLE_WINDOW_TINT(veh);

		if (carModified != 3)
		{
			ReplaceVehicle(veh, true);
			SET_VEHICLE_WINDOW_TINT(veh, 3);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_RANDTRAFFIC, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Random Traffic",
		.Id = "veh_randtraffic",
		.IsTimed = true
	}
);