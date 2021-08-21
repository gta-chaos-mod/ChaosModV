#include <stdafx.h>
//effect by ProfessorBiddle, this is a bit clunky but it works well.

#include "Memory/Hooks/HandleToEntityStructHook.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		//detect if vehicle already randomized
		auto carModified = GET_VEHICLE_WINDOW_TINT(veh);

		if(carModified != 3)
		{
			//remove ped from vehicle and place them out of the way for now
			Ped pedInCar = GET_PED_IN_VEHICLE_SEAT(veh, -1, 0);
			Vector3 pedCoords = GET_ENTITY_COORDS(pedInCar, false);
			SET_ENTITY_COORDS(pedInCar, pedCoords.x, pedCoords.y, pedCoords.z + 30.f, 1, 0, 0, 1);

			//spawn random vehicle in place of old vehicle
			Vector3 prevPos = GET_ENTITY_COORDS(veh, false);
			float prevRot = GET_ENTITY_HEADING(veh);
			Vector3 prevVel = GET_ENTITY_VELOCITY(veh);
			float prevForward = GET_ENTITY_SPEED(veh);
			bool prevWasMissionEntity = IS_ENTITY_A_MISSION_ENTITY(veh);
			SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);

			Vehicle oldVehHandle = veh;
			if (prevWasMissionEntity)
			{
				Hooks::EnableScriptThreadBlock();
			}
			DELETE_VEHICLE(&veh);
			static const std::vector<Hash>& vehModels = Memory::GetAllVehModels();
			if (!vehModels.empty())
			{
				Vehicle veh = CreateTempVehicle(vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)], prevPos.x, prevPos.y, prevPos.z, prevRot);

				if (prevWasMissionEntity)
				{
					Hooks::ProxyEntityHandle(oldVehHandle, veh);
					Hooks::DisableScriptThreadBlock();
				}

				// Also apply random upgrades
				SET_VEHICLE_MOD_KIT(veh, 0);
				for (int i = 0; i < 50; i++)
				{
					int max = GET_NUM_VEHICLE_MODS(veh, i);
					SET_VEHICLE_MOD(veh, i, max > 0 ? g_Random.GetRandomInt(0, max - 1) : 0, g_Random.GetRandomInt(0, 1));
					TOGGLE_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, 1));
				}
				SET_VEHICLE_TYRES_CAN_BURST(veh, g_Random.GetRandomInt(0, 1));

				//way to know if vehicle is modified
				SET_VEHICLE_WINDOW_TINT(veh, 3);

				//set ped back into vehicle
				SET_PED_INTO_VEHICLE(pedInCar, veh, -1);

				SET_VEHICLE_ENGINE_ON(veh, true, true, false);
				SET_ENTITY_VELOCITY(veh, prevVel.x, prevVel.y, prevVel.z);
				SET_VEHICLE_FORWARD_SPEED(veh, prevForward);

				if (!IS_PED_A_PLAYER(pedInCar))
				{
					TASK_VEHICLE_DRIVE_WANDER(pedInCar, veh, 40, 786603);
				}
			}
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