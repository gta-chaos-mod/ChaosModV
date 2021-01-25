/*
	Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Memory/Hooks/HandleToEntityStructHook.h"

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	static Vehicle lastVeh = 0;

	static const std::vector<Hash> vehModels = Memory::GetAllVehModels();
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
			Vehicle currentVehicle = GET_VEHICLE_PED_IS_IN(playerPed, false);
			int numberOfSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(currentVehicle));

			isEngineRunning = GET_IS_VEHICLE_ENGINE_RUNNING(currentVehicle);
			newVehCoords = GET_ENTITY_COORDS(currentVehicle, false);
			heading = GET_ENTITY_HEADING(currentVehicle);
			vehVelocity = GET_ENTITY_VELOCITY(currentVehicle);
			forwardSpeed = GET_ENTITY_SPEED(currentVehicle);

			for (int i = -1; i < numberOfSeats - 1; i++)
			{
				if (IS_VEHICLE_SEAT_FREE(currentVehicle, i, false))
				{
					continue;
				}
				Ped ped = GET_PED_IN_VEHICLE_SEAT(currentVehicle, i, false);
				vehPeds.push_back(ped);
			}

			if (IS_ENTITY_A_MISSION_ENTITY(currentVehicle))
			{
				oldVehHandle = currentVehicle;
			}

			SET_ENTITY_AS_MISSION_ENTITY(currentVehicle, true, true);
			DELETE_VEHICLE(&currentVehicle);
		}
		else
		{
			heading = GET_ENTITY_HEADING(playerPed);
			vehPeds.push_back(playerPed);
			newVehCoords = GET_ENTITY_COORDS(playerPed, false);
			vehVelocity = GET_ENTITY_VELOCITY(playerPed);
			forwardSpeed = GET_ENTITY_SPEED(playerPed);
		}

		Hash randomVeh = vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)];
		while (GET_VEHICLE_MODEL_NUMBER_OF_SEATS(randomVeh) < vehPeds.size())
		{
			randomVeh = vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)];
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

		if (lastVeh)
		{
			DELETE_VEHICLE(&lastVeh);
		}

		lastVeh = newVehicle;

		SET_ENTITY_AS_MISSION_ENTITY(newVehicle, false, true);
		SET_MODEL_AS_NO_LONGER_NEEDED(randomVeh);

		if (oldVehHandle)
		{
			Hooks::ProxyEntityHandle(oldVehHandle, newVehicle);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_REPLACEVEHICLE, OnStart);
