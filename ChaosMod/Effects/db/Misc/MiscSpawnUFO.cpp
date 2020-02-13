#include <stdafx.h>

static void OnStart()
{
	auto playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static const Hash model = GET_HASH_KEY("p_spinning_anus_s");
	LoadModel(model);

	auto prop = CREATE_OBJECT(model, playerPos.x, playerPos.y, playerPos.z, true, false, true);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

static RegisterEffect registerEffect(EFFECT_SPAWN_UFO, OnStart);