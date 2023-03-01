#include <stdafx.h>

#include "Memory/WeaponPool.h"

static std::map<Ped, DWORD64> pedPainMap;

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
	}
	pedPainMap.clear();
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	for (Ped ped : GetAllPeds())
	{
		if (ped == playerPed) continue;

		SET_RAGDOLL_BLOCKING_FLAGS(ped, 1);
		_SET_PED_SCREAM(ped); // Gives the ped a voice. Some peds don't have this for some reason.

		if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(ped) && !pedPainMap.contains(ped) && !IS_PED_DEAD_OR_DYING(ped, true))
		{
			SET_PED_TO_RAGDOLL(ped, 4000, 5000, 1, 1, 1, 0);
			CREATE_NM_MESSAGE(true, 0);
			GIVE_PED_NM_MESSAGE(ped);
			CREATE_NM_MESSAGE(true, 1151);
			GIVE_PED_NM_MESSAGE(ped);

			pedPainMap.emplace(ped, GET_GAME_TIMER());
			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}
	}
	for (auto it = pedPainMap.cbegin(); it != pedPainMap.cend();)
	{
		if ((!DOES_ENTITY_EXIST(it->first) || IS_ENTITY_DEAD(it->first, false))
		    || GET_GAME_TIMER() - it->second >= 3800)
		{
			it = pedPainMap.erase(it);
			continue;
		}
		PLAY_PAIN(it->first, 8, 0, 0);
		it++;
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Drama Queen",
		.Id = "peds_screamonshot",
		.IsTimed = true,
		.IncompatibleWith = { "player_noragdoll", "player_ragdollondmg" },
	}
);