/*
       Effect by ProfessorBiddle, modified
*/

#include <stdafx.h>

static int lastPlayerKills, fade, alpha, orange;
static Hash currentPedHash;
static Hash bladeHash;

static void OnStart()
{
	currentPedHash = -1;
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
	if (playerHash != currentPedHash)
	{
		currentPedHash = playerHash;
		lastPlayerKills = -1;
	}

	Hash killHash;
	//get correct character hash
	switch (playerHash)
	{
	case 225514697: // Michael 
		killHash = GET_HASH_KEY("SP0_KILLS");
		break;
	case 2602752943: // Franklin
		killHash = GET_HASH_KEY("SP1_KILLS");
		break;
	case 2608926626: // Trevor
		killHash = GET_HASH_KEY("SP2_KILLS");
		break;
	}

	//get stat for current character
	int playerKills;
	STAT_GET_INT(killHash, &playerKills, -1);
	//check if stat this tick is larger than stat last tick
	if (lastPlayerKills >= 0 && playerKills > lastPlayerKills)
	{
		fade = 0;
		alpha = 0;
	}
	lastPlayerKills = playerKills;

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
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_VEH_SPEED_MINIMUM }
	}
);