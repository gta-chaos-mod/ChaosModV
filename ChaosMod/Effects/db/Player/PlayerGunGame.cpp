
#include <stdafx.h>

//Effect by ProfessorBiddle
int kills;
Hash killsHash = GET_HASH_KEY("SP0_KILLS");
static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	REMOVE_ALL_PED_WEAPONS(playerPed, 0);
	GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
	switch (GET_ENTITY_MODEL(playerPed))
	{
	case 225514697: // Michael 
		kills = STAT_GET_INT(GET_HASH_KEY())
		break;
	case 2602752943: // Franklin

		break;
	case 2608926626: // Trevor

		break;
	}

}

static void OnTick()
{

















	Weapon wepGive;
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_SHOOTING(playerPed))
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


}
static RegisterEffect registerEffect(EFFECT_PLAYER_GUN_GAME, OnStart, nullptr, OnTick);