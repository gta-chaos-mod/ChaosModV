#include <stdafx.h>
//Effect by ProfessorBiddle, requested thousands of times on discord

int playerHits, playerHitsLast;

static void OnStart()
{
	playerHits = -1;
	//set to effectively infinite to prevent death on activation
	playerHitsLast = 2147483647;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	Hash playerHash = GET_ENTITY_MODEL(playerPed);
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
	STAT_GET_INT(hitHash, &playerHits, -1);
	//check if stat this tick is larger than stat last tick
	if (playerHits > playerHitsLast)
	{
		START_ENTITY_FIRE(playerPed);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
	}
	playerHitsLast = playerHits;
}
static RegisterEffect registerEffect(EFFECT_PLAYER_PACIFIST, OnStart, nullptr, OnTick);