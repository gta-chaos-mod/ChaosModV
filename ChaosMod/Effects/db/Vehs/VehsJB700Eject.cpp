/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

static void EjectPed(int seat, Vehicle veh)
{
	Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, seat, 0);

	CLEAR_PED_TASKS_IMMEDIATELY(ped);
	SET_ENTITY_NO_COLLISION_ENTITY(ped, veh, 1);
	SET_PED_TO_RAGDOLL(ped, 1000, 1000, 0, true, true, false);
	APPLY_FORCE_TO_ENTITY(ped, 1, 0, 0, 10, 2, 0, 0, 0, true, true, true, false, true);
}

static void OnStart()
{
	Ped plrPed = PLAYER_PED_ID();
	Vehicle veh = GET_VEHICLE_PED_IS_USING(plrPed);

	Hash vehHash = GET_ENTITY_MODEL(veh);
	int seats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehHash);

	std::vector<int> seatsToEject;

	for (int i = -1; i < seats - 1; i++)
	{
		if (!IS_VEHICLE_SEAT_FREE(veh, i, 1))
		{
			seatsToEject.push_back(i);
		}
	}

	for (int i = 0; i < seatsToEject.size(); i++)
	{
		EjectPed(seatsToEject[i], veh);
	}

}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_VEHS_JB700_EJECT, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Eject!!",
		.Id = "vehs_jb700_eject",
	}
);