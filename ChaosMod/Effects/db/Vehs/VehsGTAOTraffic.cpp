#include <stdafx.h>

static void OnTick()
{
	static std::vector<Ped> goneThrough;

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false) && GET_PED_IN_VEHICLE_SEAT(GET_VEHICLE_PED_IS_IN(ped, false), -1, 0) == ped
			&& std::find(goneThrough.begin(), goneThrough.end(), ped) == goneThrough.end())
		{
			Vehicle veh = GET_VEHICLE_PED_IS_IN(ped, false);

			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

			TASK_VEHICLE_MISSION_PED_TARGET(ped, veh, playerPed, 13, 9999.f, 4176732, .0f, .0f, false);

			goneThrough.push_back(ped);
		}
	}

	std::vector<Ped>::iterator it;
	for (it = goneThrough.begin(); it != goneThrough.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = goneThrough.erase(it);
		}
		else
		{
			it++;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_GTAO_TRAFFIC, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Traffic Magnet",
		.Id = "traffic_gtao",
		.IsTimed = true
	}
);