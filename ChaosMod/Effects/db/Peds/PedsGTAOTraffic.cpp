#include <stdafx.h>

static void OnTick()
{
	static std::vector<Ped> goneThrough;

	auto playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false) && GET_PED_IN_VEHICLE_SEAT(GET_VEHICLE_PED_IS_IN(ped, false), -1, 0) == ped
			&& std::find(goneThrough.begin(), goneThrough.end(), ped) == goneThrough.end())
		{
			TASK_VEHICLE_GOTO_NAVMESH(ped, GET_VEHICLE_PED_IS_IN(ped, false), playerPos.x, playerPos.y, playerPos.z, 9999.f, 156, 0.f);

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

static RegisterEffect registerEffect(EFFECT_GTAO_TRAFFIC, nullptr, nullptr, OnTick);