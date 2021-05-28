#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	bool playerPedInAnyVehicle = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	std::vector<Vehicle> vehs;

	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			vehs.push_back(veh);
		}
	}

	if (!vehs.empty())
	{
		Hash playerVehModel = GET_ENTITY_MODEL(playerVeh);
		int playerVehMaxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(playerVehModel);

		// Store all ped in current vehicle (if existant) to set them into chosen vehicle afterwards if possible

		std::vector<Ped> teleportPeds;
		teleportPeds.push_back(playerPed);

		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			for (int i = -1; i < playerVehMaxSeats - 1; i++)
			{
				if (!IS_VEHICLE_SEAT_FREE(playerVeh, i, false))
				{
					Ped seatPed = GET_PED_IN_VEHICLE_SEAT(playerVeh, i, false);

					if (seatPed != playerPed)
					{
						teleportPeds.push_back(seatPed);
					}
				}
			}
		}

		Vehicle targetVeh = vehs[g_random.GetRandomInt(0, vehs.size() - 1)];
		Vector3 vehLocation = GET_ENTITY_COORDS(targetVeh, true);

		float groundZ;
		bool useGroundZ;
		for (int i = 0; i < 100; i++)
		{
			float testZ = (i * 10.f) - 100.f;

			if (i % 5 == 0)
			{
				WAIT(0);
			}

			useGroundZ = GET_GROUND_Z_FOR_3D_COORD(vehLocation.x, vehLocation.y, testZ, &groundZ, false, false);
			if (useGroundZ)
			{
				break;
			}
		}
		if (groundZ > vehLocation.z)
		{
			SET_ENTITY_COORDS(targetVeh, vehLocation.x, vehLocation.y, groundZ, 1, 0, 0, 1);
		}
		
		Hash targetVehModel = GET_ENTITY_MODEL(targetVeh);
		int targetVehMaxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(targetVehModel);

		for (int i = 0; i < teleportPeds.size(); i++)
		{
			if (i >= targetVehMaxSeats)
			{
				break;
			}

			Ped ped = teleportPeds[i];

			if (!IS_VEHICLE_SEAT_FREE(targetVeh, i - 1, false))
			{
				Ped seatPed = GET_PED_IN_VEHICLE_SEAT(targetVeh, i - 1, false);

				CLEAR_PED_TASKS_IMMEDIATELY(seatPed);

				WAIT(0);
			}

			SET_PED_INTO_VEHICLE(ped, targetVeh, i - 1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SETINTORANDVEH, OnStart, EffectInfo
	{
		.Name = "Set Player Into Random Vehicle",
		.Id = "player_setintorandveh"
	}
);