#include <stdafx.h>

static void SpawnProp(const char* propName)
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static const Hash model = GET_HASH_KEY(propName);
	LoadModel(model);

	Object prop = CREATE_OBJECT(model, playerPos.x, playerPos.y, playerPos.z, true, false, true);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

static void OnStartUFO()
{
	SpawnProp("p_spinning_anus_s");
}

static RegisterEffect registerEffect(EFFECT_SPAWN_UFO, OnStartUFO);

static void OnStartFerrisWheel()
{
	SpawnProp("prop_ld_ferris_wheel");
}

static RegisterEffect registerEffect2(EFFECT_SPAWN_FERRISWHEEL, OnStartFerrisWheel);