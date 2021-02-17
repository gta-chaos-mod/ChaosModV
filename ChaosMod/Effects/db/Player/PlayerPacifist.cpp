#include <stdafx.h>
//Effect by ProfessorBiddle, requested thousands of times on discord
static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	Hash playerHash = GET_ENTITY_MODEL(playerPed);
	Hash hitHash;
	int playerHits, playerHitsNew;

	//get current hits
	switch (playerHash)
	{
	case 225514697: // Michael 
		hitHash = GET_HASH_KEY("SP0_HITS");
		break;
	case 2602752943: // Franklin
		hitHash = GET_HASH_KEY("SP0_HITS");
		break;
	case 2608926626: // Trevor
		hitHash = GET_HASH_KEY("SP0_HITS");
		break;
	}
	STAT_GET_INT(hitHash, &playerHits, -1);

	WAIT(0);

	//check if player changed
	playerPed = PLAYER_PED_ID();
	Hash newPlayerHash = GET_ENTITY_MODEL(playerPed);
	if (newPlayerHash = playerHash)
	{
		switch (playerHash)
		{
		case 225514697: // Michael 
			hitHash = GET_HASH_KEY("SP0_HITS");
			break;
		case 2602752943: // Franklin
			hitHash = GET_HASH_KEY("SP0_HITS");
			break;
		case 2608926626: // Trevor
			hitHash = GET_HASH_KEY("SP0_HITS");
			break;
		}
		STAT_GET_INT(hitHash, &playerHitsNew, -1);
		if (playerHitsNew > playerHits)
		{
			START_ENTITY_FIRE(playerPed);
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
	}
}
static RegisterEffect registerEffect(EFFECT_PLAYER_PACIFIST, nullptr, nullptr, OnTick);