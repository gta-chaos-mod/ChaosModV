#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		WAIT(0);

		if (!IS_PED_IN_ANY_VEHICLE(ped, true))
		{
			continue;
		}

		Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
		SET_ENTITY_COORDS(ped, pedPos.x, pedPos.y, pedPos.z + 5.f, false, false, false, false);
	}

	WAIT(0);

	for (Ped ped : GetAllPeds())
	{
		// May crash, if you try to put a whale into the driver seat + ped may not be valid anymore after several
		// "WAIT"s
		if (!DOES_ENTITY_EXIST(ped) || !IS_PED_HUMAN(ped))
		{
			continue;
		}

		std::vector<Vehicle> vehs;
		for (Vehicle veh : GetAllVehs())
		{
			if (ARE_ANY_VEHICLE_SEATS_FREE(veh) && IS_VEHICLE_DRIVEABLE(veh, true)
			    && GET_ENTITY_HEIGHT_ABOVE_GROUND(veh) >= 0)
			{
				vehs.push_back(veh);
			}
		}

		if (vehs.empty())
		{
			return;
		}

		Vehicle targetVeh     = vehs[g_Random.GetRandomInt(0, vehs.size() - 1)];
		Hash targetVehModel   = GET_ENTITY_MODEL(targetVeh);
		int targetVehMaxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(targetVehModel);
		for (int i = -1; i < targetVehMaxSeats - 1; i++)
		{
			if (IS_VEHICLE_SEAT_FREE(targetVeh, i, false))
			{
				SET_PED_INTO_VEHICLE(ped, targetVeh, i);

				break;
			}
		}

		WAIT(0);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Set Everyone Into Random Vehicles",
		.Id = "peds_intorandomvehs"
	}
);