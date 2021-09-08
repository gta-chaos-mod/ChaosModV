#include <stdafx.h>

static void SpawnProp(const char* propName)
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	Hash model = GET_HASH_KEY(propName);
	LoadModel(model);
	
	Vector3 min, max;
	GET_MODEL_DIMENSIONS(model, &min, &max);
	float height = (max - min).z;

	Vector3 spawnPos = playerPos;
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		Object prop = CreatePoolProp(model, spawnPos.x, spawnPos.y, spawnPos.z, true);
		spawnPos.z += height;
	}

	SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

static void OnStartUFO()
{
	SpawnProp("p_spinning_anus_s");
}

static RegisterEffect registerEffect(EFFECT_SPAWN_UFO, OnStartUFO, EffectInfo
	{
		.Name = "Spawn UFO",
		.Id = "misc_spawnufo",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
static void OnStartFerrisWheel()
{
	SpawnProp("prop_ld_ferris_wheel");
}

static RegisterEffect registerEffect2(EFFECT_SPAWN_FERRISWHEEL, OnStartFerrisWheel, EffectInfo
	{
		.Name = "Spawn Ferris Wheel",
		.Id = "misc_spawnferriswheel",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
	}
);