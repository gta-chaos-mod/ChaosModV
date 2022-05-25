#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		std::vector<Ped> vehPeds;

		Hash vehModel = GET_ENTITY_MODEL(playerVeh);
		int maxSeats  = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);
		for (int i = -1; i < maxSeats - 1; i++)
		{
			if (IS_VEHICLE_SEAT_FREE(playerVeh, i, false))
			{
				continue;
			}

			Ped ped = GET_PED_IN_VEHICLE_SEAT(playerVeh, i, false);

			CLEAR_PED_TASKS_IMMEDIATELY(ped);

			SET_PED_TO_RAGDOLL(ped, 5000, 5000, 0, true, true, false);

			vehPeds.push_back(ped);
		}

		Vector3 vehVel = GET_ENTITY_VELOCITY(playerVeh);

		SET_ENTITY_AS_MISSION_ENTITY(playerVeh, true, true);
		DELETE_VEHICLE(&playerVeh);

		WAIT(0);

		for (Ped ped : vehPeds)
		{
			SET_ENTITY_VELOCITY(ped, vehVel.x, vehVel.y, vehVel.z);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Remove Current Vehicle",
		.Id = "playerveh_despawn"
	}
);