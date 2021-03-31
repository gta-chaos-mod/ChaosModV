#include <stdafx.h>

static void OnStart()
{
	static const Hash faggioHash = GET_HASH_KEY("FAGGIO");

	Ped playerPed = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_PED_DEAD_OR_DYING(ped, false))
		{
			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);

				if (GET_ENTITY_MODEL(veh) == faggioHash)
				{
					continue;
				}
			}

			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
			float pedHeading = GET_ENTITY_HEADING(ped);

			SET_ENTITY_COORDS(ped, pedPos.x, pedPos.y, pedPos.z + 3.f, false, false, false, false);
			SET_PED_COMBAT_ATTRIBUTES(ped, 3, false); // Don't allow them to leave vehicle by themselves
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

			Vehicle veh = CreateTempVehicle(faggioHash, pedPos.x, pedPos.y, pedPos.z, pedHeading);
			SET_PED_INTO_VEHICLE(ped, veh, -1);

			TASK_VEHICLE_MISSION_PED_TARGET(ped, veh, playerPed, 13, 9999.f, 4176732, .0f, .0f, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_SCOOTERBROTHERS, OnStart, EffectInfo
	{
		.Name = "Scooter Brothers",
		.Id = "peds_scooterbrothers"
	}
);