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
		auto playerPed = PLAYER_PED_ID();

		Vehicle veh = vehs[Random::GetRandomInt(0, vehs.size() - 1)];
		if (veh != GET_VEHICLE_PED_IS_IN(playerPed, false))
		{
			SET_PED_INTO_VEHICLE(playerPed, veh, -1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SETINTORANDVEH, OnStart);