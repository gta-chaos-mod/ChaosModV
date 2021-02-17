#include <stdafx.h>
//Effect by ProfessorBiddle, requested thousands of times on discord
static void OnTick()
{
	int hits0, hits1, hits2;
	STAT_GET_INT(GET_HASH_KEY("SP0_HITS"), &hits0, -1);
	STAT_GET_INT(GET_HASH_KEY("SP1_HITS"), &hits1, -1);
	STAT_GET_INT(GET_HASH_KEY("SP2_HITS"), &hits2, -1);
	
	WAIT(0);

	Ped playerPed = PLAYER_PED_ID();

	int hits0Post, hits1Post, hits2Post;

	switch (GET_ENTITY_MODEL(playerPed))
	{
	case 225514697: // Michael 
		STAT_GET_INT(GET_HASH_KEY("SP0_HITS"), &hits0Post, -1);
		if (hits0Post > hits0)
		{
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
		break;
	case 2602752943: // Franklin
		STAT_GET_INT(GET_HASH_KEY("SP1_HITS"), &hits1Post, -1);
		if (hits1Post > hits1)
		{
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
		break;
	case 2608926626: // Trevor
		STAT_GET_INT(GET_HASH_KEY("SP2_HITS"), &hits2Post, -1);
		if (hits2Post > hits2)
		{
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
		break;
	}
}
static RegisterEffect registerEffect(EFFECT_PLAYER_PACIFIST, nullptr, nullptr, OnTick);