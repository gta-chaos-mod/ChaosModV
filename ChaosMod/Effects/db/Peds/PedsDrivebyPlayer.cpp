#include <stdafx.h>

static std::vector<Ped> m_ranThroughPeds;

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	Hash weaponHash = GET_HASH_KEY("WEAPON_MACHINEPISTOL");

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			m_ranThroughPeds.push_back(ped);

			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

			GIVE_WEAPON_TO_PED(ped, weaponHash, 9999, true, true);
			TASK_DRIVE_BY(ped, playerPed, 0, 0, 0, 0, (float)-1, 5, false, 0xC6EE6B4C);
		}
	}
}

static void OnStop()
{
	m_ranThroughPeds.clear();
}

static RegisterEffect registerEffect(EFFECT_PEDS_DRIVEBY, nullptr, OnStop, OnTick);