#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void SpawnProp(Hash propHash)
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	LoadModel(propHash);

	Object prop = CreatePoolProp(propHash, playerPos.x, playerPos.y, playerPos.z, true);
	SET_MODEL_AS_NO_LONGER_NEEDED(propHash);
}

static void OnStartUFO()
{
	SpawnProp("p_spinning_anus_s"_hash);
}

// clang-format off
REGISTER_EFFECT(OnStartUFO, nullptr, nullptr, 
	{
		.Name = "Spawn UFO",
		.Id = "misc_spawnufo",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartFerrisWheel()
{
	SpawnProp("prop_ld_ferris_wheel"_hash);
}

// clang-format off
REGISTER_EFFECT(OnStartFerrisWheel, nullptr, nullptr, 
	{
		.Name = "Spawn Ferris Wheel",
		.Id = "misc_spawnferriswheel",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);