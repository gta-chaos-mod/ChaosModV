#include <stdafx.h>

static void OnTick()
{
	static std::vector<Ped> ranThroughPeds;

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			ranThroughPeds.push_back(ped);

			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

			GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MACHINEPISTOL"), 9999, true, true);
			TASK_DRIVE_BY(ped, PLAYER_PED_ID(), 0, 0, 0, 0, (float)-1, 100, false, 0xC6EE6B4C);
		}
	}

	std::vector<Ped>::iterator it;
	for (it = ranThroughPeds.begin(); it != ranThroughPeds.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = ranThroughPeds.erase(it);
		}
		else
		{
			it++;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_DRIVEBY, nullptr, nullptr, OnTick);