#include <stdafx.h>

static void SpawnProp(const char *propName)
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	Hash model        = GET_HASH_KEY(propName);
	LoadModel(model);

	Object prop = CreatePoolProp(model, playerPos.x, playerPos.y, playerPos.z, true);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

static void OnStartUFO()
{
	SpawnProp("p_spinning_anus_s");
}

// clang-format off
REGISTER_EFFECT(OnStartUFO, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn UFO",
		.Id = "misc_spawnufo",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartFerrisWheel()
{
	SpawnProp("prop_ld_ferris_wheel");
}

// clang-format off
REGISTER_EFFECT(OnStartFerrisWheel, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Ferris Wheel",
		.Id = "misc_spawnferriswheel",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);