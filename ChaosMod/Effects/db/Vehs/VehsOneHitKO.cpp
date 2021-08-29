/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static std::vector<Vehicle> explodedVehicles;

static void OnStop()
{
	for (Vehicle veh : GetAllVehs())
	{
		Memory::SetVehicleOutOfControl(veh, false);
	}

	explodedVehicles.clear();
}

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 1000)
	{
		lastTick = curTick;

		for (Vehicle veh : GetAllVehs())
		{
			Memory::SetVehicleOutOfControl(veh, true);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (DOES_ENTITY_EXIST(veh) && IS_ENTITY_DEAD(veh, false) && std::find(explodedVehicles.begin(), explodedVehicles.end(), veh) == explodedVehicles.end())
		{
			for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier - 1; i++)
			{
				WAIT(100); // Wait a tiny bit to make the effect clearer
				Vector3 vehCoords = GET_ENTITY_COORDS(veh, false);
				ADD_EXPLOSION(vehCoords.x, vehCoords.y, vehCoords.z, 4, 100.f, true, false, 1.f, false);
			}

			explodedVehicles.push_back(veh);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_OHKO_VEHICLES, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Vehicles Explode On Impact",
		.Id = "vehs_ohko",
		.IsTimed = true
	}
);