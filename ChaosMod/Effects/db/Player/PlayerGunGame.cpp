#include <stdafx.h>

//Effect by ProfessorBiddle

int playerKills, playerKillsLast;

static void OnStart()
{
	playerKills = -1;
	//set to effectively infinite to prevent death on activation
	playerKillsLast = 2147483647;
	Ped playerPed = PLAYER_PED_ID();
	REMOVE_ALL_PED_WEAPONS(playerPed, 0);
	GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
}

static void OnTick()
{
	Weapon wepGive;
	Ped playerPed = PLAYER_PED_ID();

	Hash playerHash = GET_ENTITY_MODEL(playerPed);
	Hash hitHash;
	//get correct character hash
	switch (playerHash)
	{
	case 225514697: // Michael 
		hitHash = GET_HASH_KEY("SP0_KILLS");
		break;
	case 2602752943: // Franklin
		hitHash = GET_HASH_KEY("SP1_KILLS");
		break;
	case 2608926626: // Trevor
		hitHash = GET_HASH_KEY("SP2_KILLS");
		break;
	}
	//get stat for current character
	STAT_GET_INT(hitHash, &playerKills, -1);
	//check if stat this tick is larger than stat last tick
	if (playerKills > playerKillsLast)
	{
		REMOVE_ALL_PED_WEAPONS(playerPed, 0);
		static const std::vector<Hash>& weps = Memory::GetAllWeapons();
		wepGive = weps[g_random.GetRandomInt(0, weps.size() - 1)];
		//make sure it's actually a gun, not a broken bottle etc.
		while (GET_WEAPON_CLIP_SIZE(wepGive) < 2)
		{
			wepGive = weps[g_random.GetRandomInt(0, weps.size() - 1)];
		}
		//player always has pistol because sometimes the other weapon doesn't work
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
		GIVE_WEAPON_TO_PED(playerPed, wepGive, 9999, false, true);
	}
	playerKillsLast = playerKills;
}

static RegisterEffect registerEffect(EFFECT_PLAYER_GUN_GAME, OnStart, nullptr, OnTick);