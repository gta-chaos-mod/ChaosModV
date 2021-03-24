#include <stdafx.h>
//Effect by ProfessorBiddle, requested thousands of times on discord

static int lastPlayerHits;
static Hash currentPedHash;

static void OnStart()
{
	currentPedHash = -1;
	lastPlayerHits = -1;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	Hash playerHash = GET_ENTITY_MODEL(playerPed);
	if (playerHash != currentPedHash)
	{
		currentPedHash = playerHash;
		lastPlayerHits = -1;
	}

	Hash hitHash;
	//get correct character hash
	switch (playerHash)
	{
	case 225514697: // Michael 
		hitHash = GET_HASH_KEY("SP0_HITS");
		break;
	case 2602752943: // Franklin
		hitHash = GET_HASH_KEY("SP1_HITS");
		break;
	case 2608926626: // Trevor
		hitHash = GET_HASH_KEY("SP2_HITS");
		break;
	}

	//get stat for current character
	int playerHits;
	STAT_GET_INT(hitHash, &playerHits, -1);
	//check if stat this tick is larger than stat last tick
	if (lastPlayerHits >= 0 && playerHits > lastPlayerHits)
	{
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
	}
	lastPlayerHits = playerHits;
}
static RegisterEffect registerEffect(EFFECT_PLAYER_PACIFIST, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Pacifist",
		.Id = "player_pacifist",
		.IsTimed = true
	}
);