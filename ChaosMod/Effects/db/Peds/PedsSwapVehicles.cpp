#include <stdafx.h>

/*
 *	Effect by juhana
 */

static void OnStart()
{
	Ped player = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (ped == player)
		{
			continue;
		}

		Vehicle currentVeh = GET_VEHICLE_PED_IS_IN(ped, false);
		Vehicle closestVeh = -1;
		float closestDist = 9999.f;
		Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

		for (Vehicle veh : GetAllVehs())
		{
			if (veh == currentVeh)
			{
				continue;
			}

			Vector3 coords = GET_ENTITY_COORDS(veh, false);
			float dist = GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, pedPos.x, pedPos.y, pedPos.z, true);

			if (dist < closestDist)
			{
				closestVeh = veh;
				closestDist = dist;
			}
		}

		if (closestVeh != -1)
		{
			TASK_ENTER_VEHICLE(ped, closestVeh, -1, -2, 2.0f, 1, 0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_VEHICLE_SWAP, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Vehicle Swap",
		.Id = "peds_vehicle_swap",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
