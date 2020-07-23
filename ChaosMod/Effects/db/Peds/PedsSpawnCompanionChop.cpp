#include <stdafx.h>

static void OnStart()
{
	static const Hash modelHash = GET_HASH_KEY("a_c_chop");
	LoadModel(modelHash);

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CHOP", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	Ped ped = CreatePoolPed(28, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f);
	SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);
	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));

	SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
}

static RegisterEffect registerEffect(EFFECT_SPAWN_COMPANION_CHOP, OnStart);