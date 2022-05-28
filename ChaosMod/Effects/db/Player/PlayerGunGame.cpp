/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static int currentTier  = 0;
static int lastPlayerKills;

static const int checkInterval = 1000;

//Going from worst to best
std::vector<Hash> tiers = 
{ 
	GET_HASH_KEY("WEAPON_UNARMED"),
	GET_HASH_KEY("WEAPON_KNIFE"),
	GET_HASH_KEY("WEAPON_PISTOL"),
	GET_HASH_KEY("WEAPON_MICROSMG"),
	GET_HASH_KEY("WEAPON_ASSAULTRIFLE"),
	GET_HASH_KEY("WEAPON_PUMPSHOTGUN"),
	GET_HASH_KEY("WEAPON_SNIPERRIFLE"),
	GET_HASH_KEY("WEAPON_COMBATMG"),
	GET_HASH_KEY("WEAPON_MINIGUN"),
	GET_HASH_KEY("WEAPON_RAILGUN"),
	GET_HASH_KEY("WEAPON_GADGETPISTOL"), // Perico Pistol
};

static void OnStart()
{
	currentTier = 0;
	for (Hash hash : { GET_HASH_KEY("SP0_KILLS"), GET_HASH_KEY("SP1_KILLS"), GET_HASH_KEY("SP2_KILLS") })
	{
		int curKills;
		STAT_GET_INT(hash, &curKills, -1);
		lastPlayerKills += curKills;
	}
}

static void OnTick()
{
	static const int maxTier = tiers.size() - 1;
	Ped player = PLAYER_PED_ID();

	DISABLE_CONTROL_ACTION(1, 37, 1);
	DISABLE_CONTROL_ACTION(1, 12, 1);
	DISABLE_CONTROL_ACTION(1, 13, 1);
	DISABLE_CONTROL_ACTION(1, 14, 1);
	DISABLE_CONTROL_ACTION(1, 15, 1);
	DISABLE_CONTROL_ACTION(1, 16, 1);
	DISABLE_CONTROL_ACTION(1, 17, 1);

	Hash curWep;
	GET_CURRENT_PED_WEAPON(player, &curWep, false);
	if (curWep != tiers.at(currentTier))
	{
		GIVE_WEAPON_TO_PED(player, tiers.at(currentTier), 9999, false, false);
		SET_CURRENT_PED_WEAPON(player, tiers.at(currentTier), false);
	}

	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	if (lastTick < curTick - checkInterval)
	{
		int allPlayerKills = 0;
		int curKills       = 0;
		for (Hash hash : { GET_HASH_KEY("SP0_KILLS"), GET_HASH_KEY("SP1_KILLS"), GET_HASH_KEY("SP2_KILLS") })
		{
			STAT_GET_INT(hash, &curKills, -1);
			allPlayerKills += curKills;
		}

		if (allPlayerKills > lastPlayerKills)
		{
			lastPlayerKills = allPlayerKills;
			if (currentTier >= maxTier)
			{
				//Reward the player with health, armour and a random amount of money.
				int maxHealth = GET_PED_MAX_HEALTH(player);

				SET_ENTITY_HEALTH(player, maxHealth, 0);
				ADD_ARMOUR_TO_PED(player, 200);

				for (int i = 0; i < 3; i++)
				{
					char statNameFull[32];
					sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

					auto hash = GET_HASH_KEY(statNameFull);

					int money;
					STAT_GET_INT(hash, &money, -1);
					STAT_SET_INT(hash, money + g_Random.GetRandomInt(100, 100000), 1);
				}

				currentTier = 0;
			}
			else
				currentTier++;
		}
	}

}

static void OnStop()
{
	ENABLE_CONTROL_ACTION(1, 37, 1);
	ENABLE_CONTROL_ACTION(1, 12, 1);
	ENABLE_CONTROL_ACTION(1, 13, 1);
	ENABLE_CONTROL_ACTION(1, 14, 1);
	ENABLE_CONTROL_ACTION(1, 15, 1);
	ENABLE_CONTROL_ACTION(1, 16, 1);
	ENABLE_CONTROL_ACTION(1, 17, 1);
	lastPlayerKills = 0;
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Gun Game",
		.Id = "player_gun_game",
		.IsTimed = true
	}
);