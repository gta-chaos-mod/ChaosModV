#include <stdafx.h>

//effect by ProfessorBiddle

static void OnStart()
{
	//not sure exactly what to make this, 60 seemed to work ok
	static const float maxDistance = 60.f;

	static const Hash busHash = GET_HASH_KEY("BUS");

	Ped playerPed = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_PED_DEAD_OR_DYING(ped, false))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, true);
			Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);
			//check if player is far away from entity
			if (GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, playerPos.x, playerPos.y, playerPos.z, true) <= maxDistance)
			{
				if (IS_PED_IN_ANY_VEHICLE(ped, false))
				{
					Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);

					if (GET_ENTITY_MODEL(veh) == busHash)
					{
						continue;
					}
				}

				Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
				float pedHeading = GET_ENTITY_HEADING(ped);

				SET_ENTITY_COORDS(ped, pedPos.x, pedPos.y, pedPos.z + 10.f, false, false, false, false);

				SET_PED_COMBAT_ATTRIBUTES(ped, 3, false); // Don't allow them to leave vehicle by themselves

				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

				Vehicle veh = CreateTempVehicle(busHash, pedPos.x, pedPos.y, pedPos.z, pedHeading);
				SET_PED_INTO_VEHICLE(ped, veh, -1);

				TASK_VEHICLE_MISSION_PED_TARGET(ped, veh, playerPed, 13, 9999.f, 4176732, .0f, .0f, false);
			}	
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_BUSBOIS, OnStart, EffectInfo
	{
		.Name = "Bus Bois",
		.Id = "peds_busbois"
	}
);