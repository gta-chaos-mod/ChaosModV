/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	static const Hash ballHash     = GET_HASH_KEY("prop_juicestand");
	static const Hash weaponHash   = GET_HASH_KEY("weapon_specialcarbine");
	static const int minDistance   = 2;
	static const int maxDistance   = 7;
	static const int maxSpeedCheck = 40;
	Ped player                     = PLAYER_PED_ID();
	Vector3 pos                    = GET_ENTITY_COORDS(player, false);
	// Make distance to player dependent on players speed
	float playerSpeed              = std::min(std::max(0, static_cast<int>(GET_ENTITY_SPEED(player))), maxSpeedCheck);
	float fixedDistance            = ((playerSpeed / maxSpeedCheck) * (maxDistance - minDistance)) + minDistance;
	Vector3 spawnPos               = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, fixedDistance, 0);
	Object ball                    = CreatePoolProp(ballHash, spawnPos.x, spawnPos.y, spawnPos.z - 0.2, true);
	// Randomize weight
	float weight                   = g_Random.GetRandomFloat(1, 100);
	SET_OBJECT_PHYSICS_PARAMS(ball, weight, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
	// Ball needs to be shot at to be dynamic, otherwise it will be frozen
	Vector3 min, max;
	GET_MODEL_DIMENSIONS(ballHash, &min, &max);
	SHOOT_SINGLE_BULLET_BETWEEN_COORDS(spawnPos.x, spawnPos.y, spawnPos.z + max.z - min.z, spawnPos.x, spawnPos.y,
	                                   spawnPos.z, 0, true, weaponHash, 0, false, true, 0.01);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Orange Ball",
        .Id = "misc_spawn_orange_ball",
        .EffectGroupType = EEffectGroupType::SpawnGeneric
    }
);