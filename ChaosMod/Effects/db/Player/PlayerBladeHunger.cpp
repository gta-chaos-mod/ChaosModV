/*
       Effect by ProfessorBiddle, modified
*/

#include <stdafx.h>

static int lastPlayerKills, fade, alpha, orange;
static Hash bladeHash;

static void OnStart()
{
	lastPlayerKills = -1;
	bladeHash = GET_HASH_KEY("WEAPON_MACHETE");
	fade = 0;
	alpha = 0;
	orange = 0;
}

static void OnTick()
{
	Player player = PLAYER_ID();
	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, 0);

	Ped playerPed = PLAYER_PED_ID();
	
	GIVE_WEAPON_TO_PED(playerPed, bladeHash, 1, false, true);

	Hash playerHash = GET_ENTITY_MODEL(playerPed);

	int allPlayerKills = 0;
	int curKills = 0;
	for (Hash hash : { GET_HASH_KEY("SP0_KILLS"), GET_HASH_KEY("SP1_KILLS"), GET_HASH_KEY("SP2_KILLS")})
	{
		STAT_GET_INT(hash, &curKills, -1);
		allPlayerKills += curKills;
	}

	//check if stat this tick is larger than stat last tick
	if (lastPlayerKills >= 0 && allPlayerKills > lastPlayerKills)
	{
		fade = 0;
		alpha = 0;
	}
	lastPlayerKills = allPlayerKills;

	orange += GET_RANDOM_INT_IN_RANGE(-4, 10);
	alpha += GET_RANDOM_INT_IN_RANGE(-4, 10);

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (curTick > lastTick + 150)
	{
		lastTick = curTick;
		fade++;
		alpha++;
	}

	DRAW_RECT(0.5f, 0.5f, 1.f, 1.f, fade, orange, 0, 50, 0);

	if (fade > 255)
	{
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
		fade = 0;
		alpha = 0;
		orange = 0;
	}
	if (alpha > 100)
	{
		alpha -= 20;
	}
	if (orange > 30)
	{
		orange -= 20;
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BLADE_HUNGER, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "The Blade Hungers",
		.Id = "player_blade_hunger",
		.IsTimed = true
	}
);