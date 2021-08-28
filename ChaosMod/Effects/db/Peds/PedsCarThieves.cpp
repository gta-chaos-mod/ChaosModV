#include <stdafx.h>

static std::list<Ped> carPeds;

static void OnStart()
{
	carPeds.clear();
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		Hash vehModel = GET_ENTITY_MODEL(playerVeh);
		int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);

		for (int i = -1; i < maxSeats - 1; i++)
		{
			if (!IS_VEHICLE_SEAT_FREE(playerVeh, i, false))
			{
				Ped ped = GET_PED_IN_VEHICLE_SEAT(playerVeh, i, false);
				CLEAR_PED_TASKS_IMMEDIATELY(ped);
				SET_PED_TO_RAGDOLL(ped, 3000, 3000, 0, true, true, false);
				carPeds.push_back(ped);
			}
		}

		int seatIndex = -1;
		int pedsNeeded = maxSeats;

		for (Ped ped : GetAllPeds())
		{
			bool isPrevious = false;
			for (std::list<Ped>::iterator it = carPeds.begin(); it != carPeds.end(); )
			{
				Ped carPed = *it;
				if (carPed == ped)
				{
					isPrevious = true;
				}
				it++;
			}
			if (IS_PED_HUMAN(ped) && pedsNeeded > 0 && !isPrevious)
			{
				CLEAR_PED_TASKS_IMMEDIATELY(ped);
				SET_PED_INTO_VEHICLE(ped, playerVeh, seatIndex);
				if (seatIndex == -1)
				{
					TASK_VEHICLE_DRIVE_WANDER(ped, playerVeh, 9999.f, 10);
				}
				seatIndex = -2;
				pedsNeeded--;
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_VEHTHIEVES, OnStart, EffectInfo
	{
		.Name = "Vehicle Thieves",
		.Id = "peds_vehthieves"
	}
);