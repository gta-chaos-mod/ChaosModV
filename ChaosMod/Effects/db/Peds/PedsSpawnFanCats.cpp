#include <stdafx.h>

#define FAN_CATS_AMOUNT 3

static void OnStart()
{
	static const Hash modelHash = GET_HASH_KEY("a_c_cat_01");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_FAN_CATS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	for (int i = 0; i < FAN_CATS_AMOUNT; i++)
	{
		Ped ped = CreatePoolPed(28, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Fan Cats",
		.Id = "peds_spawnfancats",
		.EffectGroupType = EEffectGroupType::SpawnCompanion
	}
);