/*
	Effect by Gorakh
*/

#include <stdafx.h>

static bool FindFreeSeat(Vehicle veh, int* seatIndex)
{
	std::vector<int> freeSeats = {};

	int numSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));
	for (int i = -1; i < numSeats; i++)
	{
		if (IS_VEHICLE_SEAT_FREE(veh, i, false) && GET_SEAT_PED_IS_TRYING_TO_ENTER(PLAYER_PED_ID()) != i)
		{
			freeSeats.push_back(i);
		}
	}

	if (freeSeats.empty())
	{
		return false;
	}

	*seatIndex = freeSeats[g_Random.GetRandomInt(0, freeSeats.size() - 1)];
	return true;
}

static std::vector<Ped> clowns = {};

static void OnTick()
{
	static int nextClownSpawnTick = 0;
	if (nextClownSpawnTick < GET_GAME_TIMER())
	{
		Vehicle vehicleToSpawnIn = -1;
		int seatIndexToSpawnIn = -2;

		Ped player = PLAYER_PED_ID();
		if (DOES_ENTITY_EXIST(player))
		{
			if (IS_PED_IN_ANY_VEHICLE(player, false))
			{
				Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);

				int seatIndex;
				if (FindFreeSeat(playerVeh, &seatIndex))
				{
					vehicleToSpawnIn = playerVeh;
					seatIndexToSpawnIn = seatIndex;
				}
			}

			if (vehicleToSpawnIn == -1) // If player is not in a vehicle, or the player's vehicle doesn't have a free seat
			{
				Vector3 playerCoords = GET_ENTITY_COORDS(player, 0);

				Vehicle closestVehicle = -1;
				int closestVehicleSeatIndex = -2;
				float closestVehicleDistance = HUGE_VALF;
				for (Vehicle veh : GetAllVehs())
				{
					Vector3 vehCoords = GET_ENTITY_COORDS(veh, 0);

					float distance = VDIST2(playerCoords.x, playerCoords.y, playerCoords.z, vehCoords.x, vehCoords.y, vehCoords.z);
					if (distance <= (60.f * 60.f)) // Distance is squared
					{
						if (closestVehicle == -1 || distance < closestVehicleDistance)
						{
							int seatIndex;
							if (FindFreeSeat(veh, &seatIndex))
							{
								closestVehicle = veh;
								closestVehicleSeatIndex = seatIndex;
								closestVehicleDistance = distance;
							}
						}
					}
				}

				if (closestVehicle != -1)
				{
					vehicleToSpawnIn = closestVehicle;
					seatIndexToSpawnIn = closestVehicleSeatIndex;
				}
			}
		}

		if (vehicleToSpawnIn != -1)
		{
			static Hash modelHash = GET_HASH_KEY("s_m_y_clown_01");
			LoadModel(modelHash);

			Ped clown = CREATE_PED_INSIDE_VEHICLE(vehicleToSpawnIn, -1, modelHash, seatIndexToSpawnIn, true, false);
			
			TASK_LEAVE_VEHICLE(clown, vehicleToSpawnIn, 4160);

			clowns.push_back(clown);

			Ped copy = clown;
			SET_PED_AS_NO_LONGER_NEEDED(&copy);

			nextClownSpawnTick = GET_GAME_TIMER() + g_Random.GetRandomInt(900, 1800);
		}
	}

	Ped player = PLAYER_PED_ID();
	if (DOES_ENTITY_EXIST(player))
	{
		for (auto it = clowns.begin(); it != clowns.end();)
		{
			Ped clown = *it;
			if (DOES_ENTITY_EXIST(clown) && !IS_ENTITY_DEAD(clown, 0))
			{
				if (!IS_PED_IN_ANY_VEHICLE(clown, true))
				{
					SET_PED_FLEE_ATTRIBUTES(clown, 2, true);
					TASK_REACT_AND_FLEE_PED(clown, player);

					it = clowns.erase(it); // Ped has left the vehicle, no reason to keep tracking it
				}
				else
				{
					it++;
				}
			}
			else
			{
				it = clowns.erase(it);
			}
		}
	}
}

static void OnStop()
{
	clowns.clear();
}

static RegisterEffect registerEffect(EFFECT_VEHS_CLOWN_CARS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Clown Cars",
		.Id = "vehs_clown_cars",
		.IsTimed = true
	}
);