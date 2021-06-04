/*
	Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Memory/Hooks/HandleToEntityStructHook.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	static const std::vector<Hash>& vehModels = Memory::GetAllVehModels();
	if (!vehModels.empty())
	{
		float heading;
		std::vector<Ped> vehPeds;
		Vector3 newVehCoords;
		bool isEngineRunning = false;
		Vector3 vehVelocity;
		float forwardSpeed = 0;
		Entity oldVehHandle = 0;
		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			oldVehHandle = GET_VEHICLE_PED_IS_IN(playerPed, false);
			int numberOfSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(oldVehHandle));

			isEngineRunning = GET_IS_VEHICLE_ENGINE_RUNNING(oldVehHandle);
			newVehCoords = GET_ENTITY_COORDS(oldVehHandle, false);
			heading = GET_ENTITY_HEADING(oldVehHandle);
			vehVelocity = GET_ENTITY_VELOCITY(oldVehHandle);
			forwardSpeed = GET_ENTITY_SPEED(oldVehHandle);

			for (int i = -1; i < numberOfSeats - 1; i++)
			{
				if (IS_VEHICLE_SEAT_FREE(oldVehHandle, i, false))
				{
					continue;
				}
				Ped ped = GET_PED_IN_VEHICLE_SEAT(oldVehHandle, i, false);
				vehPeds.push_back(ped);
			}
		}
		else
		{
			heading = GET_ENTITY_HEADING(playerPed);
			vehPeds.push_back(playerPed);
			newVehCoords = GET_ENTITY_COORDS(playerPed, false);
			vehVelocity = GET_ENTITY_VELOCITY(playerPed);
			forwardSpeed = GET_ENTITY_SPEED(playerPed);
		}

		Hash randomVeh = vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)];
		
		// Filter out Truck Trailers and other vehicles with insufficient seats
		while (GET_VEHICLE_MODEL_NUMBER_OF_SEATS(randomVeh) < vehPeds.size() || IS_THIS_MODEL_A_TRAIN(randomVeh) || GET_VEHICLE_MODEL_ACCELERATION(randomVeh) <= 0)
		{
			randomVeh = vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)];
		}
		LoadModel(randomVeh);
		Vehicle newVehicle = CREATE_VEHICLE(randomVeh, newVehCoords.x, newVehCoords.y, newVehCoords.z, heading, true, true, true);
		for (int index = 0; index < vehPeds.size(); index++)
		{
			int seatIdx = index == 0 ? -1 : -2;
			int ped = vehPeds[index];
			SET_PED_INTO_VEHICLE(ped, newVehicle, seatIdx);
		}
		if (isEngineRunning)
		{
			SET_VEHICLE_ENGINE_ON(newVehicle, true, true, false);
		}
		SET_ENTITY_VELOCITY(newVehicle, vehVelocity.x, vehVelocity.y, vehVelocity.z);
		SET_VEHICLE_FORWARD_SPEED(newVehicle, forwardSpeed);

		SET_ENTITY_AS_MISSION_ENTITY(newVehicle, false, true);
		SET_MODEL_AS_NO_LONGER_NEEDED(randomVeh);

		if (oldVehHandle)
		{
			bool shouldUseHook = IS_ENTITY_A_MISSION_ENTITY(oldVehHandle);
			Entity copy = oldVehHandle;
			SET_ENTITY_AS_MISSION_ENTITY(copy, true, true);

			if (shouldUseHook)
			{
				Hooks::EnableScriptThreadBlock();
			}
			DELETE_VEHICLE(&copy);
			if (shouldUseHook)
			{
				Hooks::ProxyEntityHandle(oldVehHandle, newVehicle);
				Hooks::DisableScriptThreadBlock();
			}
		}

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(newVehicle, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(newVehicle, i);
			SET_VEHICLE_MOD(newVehicle, i, max > 0 ? g_Random.GetRandomInt(0, max - 1) : 0, g_Random.GetRandomInt(0, 1));
			TOGGLE_VEHICLE_MOD(newVehicle, i, g_Random.GetRandomInt(0, 1));
		}
		SET_VEHICLE_TYRES_CAN_BURST(newVehicle, g_Random.GetRandomInt(0, 1));

		//way to know if vehicle is modified
		SET_VEHICLE_WINDOW_TINT(newVehicle, 3);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_REPLACEVEHICLE, OnStart, EffectInfo
	{
		.Name = "Replace Current Vehicle",
		.Id = "misc_replacevehicle"
	}
);