/*
Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
	static const float maxDistance = 60.f;
	static const Hash vehHash = GET_HASH_KEY("rhino");

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_PED_DEAD_OR_DYING(ped, false))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, true);
			//check if player is far away from entity
			if (pedPos.DistanceTo(playerPos) <= maxDistance)
			{
				if (IS_PED_IN_ANY_VEHICLE(ped, false))
				{
					Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);

					if (GET_ENTITY_MODEL(veh) == vehHash)
					{
						continue;
					}
				}

				float pedHeading = GET_ENTITY_HEADING(ped);
				Vector3 vel = GET_ENTITY_VELOCITY(ped);

				SET_ENTITY_COORDS(ped, pedPos.x, pedPos.y, pedPos.z + 10.f, false, false, false, false);

				SET_PED_COMBAT_ATTRIBUTES(ped, 3, false); // Don't allow them to leave vehicle by themselves

				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

				Vehicle veh = CreateTempVehicle(vehHash, pedPos.x, pedPos.y, pedPos.z, pedHeading);
				SET_PED_INTO_VEHICLE(ped, veh, -1);
				SET_VEHICLE_ENGINE_ON(veh, true, true, true);
				SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);

				TASK_VEHICLE_MISSION_PED_TARGET(ped, veh, playerPed, 13, 9999.f, 4176732, .0f, .0f, false);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_TANK_BOIS, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Tanks A Lot",
        .Id = "peds_tank_bois",
        .EffectGroupType = EffectGroupType::SPAWN_GENERIC
    }
);
