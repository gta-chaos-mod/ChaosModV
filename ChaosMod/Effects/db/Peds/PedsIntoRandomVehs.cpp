#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
		SET_ENTITY_COORDS(ped, pedPos.x, pedPos.y, pedPos.z + 5.f, false, false, false, false);

		WAIT(0);
	}

	WAIT(0);

	for (Ped ped : GetAllPeds())
	{
		std::vector<Vehicle> vehs;

		for (Vehicle veh : GetAllVehs())
		{
			if (ARE_ANY_VEHICLE_SEATS_FREE(veh))
			{
				vehs.push_back(veh);
			}
		}

		Vehicle targetVeh = vehs[g_random.GetRandomInt(0, vehs.size() - 1)];
		Hash targetVehModel = GET_ENTITY_MODEL(targetVeh);
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

static RegisterEffect registerEffect(EFFECT_PEDS_INTORANDOMVEHS, OnStart);