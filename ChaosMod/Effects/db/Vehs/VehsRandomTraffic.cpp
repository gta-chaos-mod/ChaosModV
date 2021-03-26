#include <stdafx.h>
//effect by ProfessorBiddle, this is a bit clunky but it works well.
static void OnTick()
{
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);
	for (auto veh : GetAllVehs())
	{
		//softlock prevention, hopefully
		if (IS_ENTITY_A_MISSION_ENTITY(veh)) { continue; }

		//detect if vehicle already randomized
		auto carModified = GET_VEHICLE_WINDOW_TINT(veh);

		if(veh != playerVeh && (carModified != 3))
		{
			//remove ped from vehicle and place them out of the way for now
			Ped pedInCar = GET_PED_IN_VEHICLE_SEAT(veh, -1, 0);
			Vector3 pedCoords = GET_ENTITY_COORDS(pedInCar, false);
			SET_ENTITY_COORDS(pedInCar, pedCoords.x, pedCoords.y, pedCoords.z + 30.f, 1, 0, 0, 1);

			//spawn random vehicle in place of old vehicle
			Vector3 prevPos = GET_ENTITY_COORDS(veh, false);
			float prevRot = GET_ENTITY_HEADING(veh);
			SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
			DELETE_VEHICLE(&veh);
			static std::vector<Hash> vehModels = Memory::GetAllVehModels();
			if (!vehModels.empty())
			{
				Vehicle veh = CreateTempVehicle(vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)], prevPos.x, prevPos.y, prevPos.z, prevRot);

				// Also apply random upgrades
				SET_VEHICLE_MOD_KIT(veh, 0);
				for (int i = 0; i < 50; i++)
				{
					int max = GET_NUM_VEHICLE_MODS(veh, i);
					SET_VEHICLE_MOD(veh, i, max > 0 ? g_random.GetRandomInt(0, max - 1) : 0, g_random.GetRandomInt(0, 1));
					TOGGLE_VEHICLE_MOD(veh, i, g_random.GetRandomInt(0, 1));
				}
				SET_VEHICLE_TYRES_CAN_BURST(veh, g_random.GetRandomInt(0, 1));

				//way to know if vehicle is modified
				SET_VEHICLE_WINDOW_TINT(veh, 3);

				//set ped back into vehicle
				SET_PED_INTO_VEHICLE(pedInCar, veh, -1);
				TASK_VEHICLE_DRIVE_WANDER(pedInCar, veh, 40, 786603);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_RANDTRAFFIC, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Random Traffic",
		.Id = "veh_randtraffic",
		.IsTimed = true,
		.IsShortDuration = true
	}
);