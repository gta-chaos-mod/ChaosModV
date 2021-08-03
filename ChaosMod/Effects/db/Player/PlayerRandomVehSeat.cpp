/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		// Set into random vehicle & seat

		std::vector<Vehicle> vehs;

		float groundZ;
		for (Vehicle veh : GetAllVehs())
		{
			Vector3 vehPos = GET_ENTITY_COORDS(veh, false);
			if (GET_GROUND_Z_FOR_3D_COORD(vehPos.x, vehPos.y, vehPos.z, &groundZ, false, false) && HAS_COLLISION_LOADED_AROUND_ENTITY(veh))
			{
				vehs.push_back(veh);
			}
		}

		if (!vehs.empty())
		{
			Ped playerPed = PLAYER_PED_ID();

			Vehicle veh = vehs[g_Random.GetRandomInt(0, vehs.size() - 1)];
			if (veh != GET_VEHICLE_PED_IS_IN(playerPed, false))
			{
				Hash vehModel = GET_ENTITY_MODEL(veh);
				int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);
				int randomSeat = g_Random.GetRandomInt(-1, maxSeats - 2);

				if (!IS_VEHICLE_SEAT_FREE(veh, randomSeat, false))
				{
					Ped seatPed = GET_PED_IN_VEHICLE_SEAT(veh, randomSeat, false);

					CLEAR_PED_TASKS_IMMEDIATELY(seatPed);

					WAIT(0);
				}

				SET_PED_INTO_VEHICLE(playerPed, veh, randomSeat);
			}
		}
	}
	else
	{
		// Set into random seat in current vehicle

		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));

		if (maxSeats >= 2)
		{
			std::vector<int> choosableSeats;
			for (int i = -1; i < maxSeats - 1; i++)
			{
				if (IS_VEHICLE_SEAT_FREE(veh, i, false) || GET_PED_IN_VEHICLE_SEAT(veh, i, false) != playerPed)
				{
					choosableSeats.push_back(i);
				}
			}

			int seat = choosableSeats[g_Random.GetRandomInt(0, choosableSeats.size() - 1)];
			if (!IS_VEHICLE_SEAT_FREE(veh, seat, false))
			{
				Ped seatPed = GET_PED_IN_VEHICLE_SEAT(veh, seat, false);

				CLEAR_PED_TASKS_IMMEDIATELY(seatPed);

				WAIT(0);
			}

			SET_PED_INTO_VEHICLE(playerPed, veh, seat);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_SET_RANDOM_SEAT, OnStart, EffectInfo
	{
		.Name = "Set Player Into Random Vehicle Seat",
		.Id = "veh_randomseat"
	}
);