/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		static std::vector<Hash> vehModels = Memory::GetAllVehModels();
		if (!vehModels.empty())
		{
			Vehicle currentVehicle = GET_VEHICLE_PED_IS_IN(playerPed, false);
			int numberOfSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(currentVehicle));

			bool isEngineRunning = GET_IS_VEHICLE_ENGINE_RUNNING(currentVehicle);
			Vector3 vehCoords = GET_ENTITY_COORDS(currentVehicle, false);
			float heading = GET_ENTITY_HEADING(currentVehicle);
			Vector3 vehVelocity = GET_ENTITY_VELOCITY(currentVehicle);
			float forwardSpeed = GET_ENTITY_SPEED(currentVehicle);

			std::vector<Ped> vehPeds;

			for (int i = -1; i < numberOfSeats - 1; i++)
			{
				if (IS_VEHICLE_SEAT_FREE(currentVehicle, i, false))
				{
					continue;
				}
				Ped ped = GET_PED_IN_VEHICLE_SEAT(currentVehicle, i, false);
				vehPeds.push_back(ped);
			}

			Hash randomVeh = vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)];
			while (GET_VEHICLE_MODEL_NUMBER_OF_SEATS(randomVeh) < vehPeds.size())
			{
				randomVeh = vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)];
			}

			SET_ENTITY_AS_MISSION_ENTITY(currentVehicle, true, true);
			DELETE_VEHICLE(&currentVehicle);

			Vehicle newVehicle = CreatePoolVehicle(randomVeh, vehCoords.x, vehCoords.y, vehCoords.z, heading);

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
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_REPLACEVEHICLE, OnStart);
