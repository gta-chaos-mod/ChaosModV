#include <stdafx.h>

static void OnStart()
{
	std::vector<Vehicle> vehs;

	for (Vehicle veh : GetAllVehs())
	{
		vehs.push_back(veh);
	}

	if (!vehs.empty())
	{
		Ped playerPed = PLAYER_PED_ID();

		Vehicle veh = vehs[g_random.GetRandomInt(0, vehs.size() - 1)];
		if (veh != GET_VEHICLE_PED_IS_IN(playerPed, false))
		{
			if (!IS_VEHICLE_SEAT_FREE(veh, -1, false))
			{
				CLEAR_PED_TASKS_IMMEDIATELY(GET_PED_IN_VEHICLE_SEAT(veh, -1, false));
				WAIT(0);
			}

			SET_PED_INTO_VEHICLE(playerPed, veh, -1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SETINTORANDVEH, OnStart);