#include <stdafx.h>

static void OnStart()
{
	Ped playerPed      = PLAYER_PED_ID();
	Vehicle playerVeh  = GET_VEHICLE_PED_IS_IN(playerPed, false);

	Vector3 playerPos  = GET_ENTITY_COORDS(playerPed, false);

	Vehicle closestVeh = -1;
	float closestDist  = 9999.f;
	for (Vehicle veh : GetAllVehs())
	{
		if (veh == playerVeh)
		{
			continue;
		}

		Vector3 coords = GET_ENTITY_COORDS(veh, false);
		float dist =
			GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
		if (dist < closestDist)
		{
			closestVeh  = veh;
			closestDist = dist;
		}
	}

	if (closestVeh != -1)
	{
		Hash playerVehModel   = GET_ENTITY_MODEL(playerVeh);
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

		Hash closestVehModel   = GET_ENTITY_MODEL(closestVeh);
		int closestVehMaxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(closestVehModel);

		for (int i = 0; i < teleportPeds.size(); i++)
		{
			if (i >= closestVehMaxSeats)
			{
				break;
			}

			Ped ped = teleportPeds[i];

			if (!IS_VEHICLE_SEAT_FREE(closestVeh, i - 1, false))
			{
				Ped seatPed = GET_PED_IN_VEHICLE_SEAT(closestVeh, i - 1, false);

				CLEAR_PED_TASKS_IMMEDIATELY(seatPed);

				WAIT(0);
			}

			SET_PED_INTO_VEHICLE(ped, closestVeh, i - 1);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Set Player Into Closest Vehicle",
		.Id = "player_tpclosestveh"
	}
);