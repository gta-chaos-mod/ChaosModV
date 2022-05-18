/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const int numRats = 7;

static void OnStart()
{
	Vector3 pos = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
	for (int i = 0; i < numRats; i++)
	{
		Ped ped = CreatePoolPed(28, GET_HASH_KEY("a_c_rat"), pos.x, pos.y, pos.z, g_Random.GetRandomFloat(0, 360));
		SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));
	}
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Spawn Rats",
		.Id = "spawn_rats",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
