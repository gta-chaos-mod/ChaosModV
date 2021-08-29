#include <stdafx.h>

static void OnStart()
{
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);

	int count = 3;

	std::vector<Vehicle> explodedVehicles;
	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			explodedVehicles.push_back(veh);

			EXPLODE_VEHICLE(veh, true, false);

			if (--count == 0)
			{
				count = 3;

				WAIT(0);
			}
		}
	}

	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier - 1; i++)
	{
		WAIT(50); // Wait a tiny bit to make the effect clearer

		count = 3;
		for (Vehicle veh : explodedVehicles)
		{
			if (veh && DOES_ENTITY_EXIST(veh))
			{
				Vector3 vehCoords = GET_ENTITY_COORDS(veh, false);
				ADD_EXPLOSION(vehCoords.x, vehCoords.y, vehCoords.z, 4, 100.f, true, false, 1.f, false);

				if (--count == 0)
				{
					count = 3;
					WAIT(0);
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLODE_VEHS, OnStart, EffectInfo
	{
		.Name = "Explode All Nearby Vehicles",
		.Id = "vehs_explode"
	}
);