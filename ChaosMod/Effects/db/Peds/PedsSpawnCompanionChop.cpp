#include <stdafx.h>

static void OnStart()
{
	static const Hash modelHash = GET_HASH_KEY("a_c_chop");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CHOP", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CreatePoolPed(28, modelHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);
	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));
}

static RegisterEffect registerEffect(EFFECT_SPAWN_COMPANION_CHOP, OnStart, EffectInfo
	{
		.Name = "Spawn Companion Doggo",
		.Id = "spawn_chop",
		.EEffectGroupType = EEffectGroupType::SpawnCompanion
	}
);