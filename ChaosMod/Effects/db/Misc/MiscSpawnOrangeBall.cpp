/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	static const Hash ballHash       = "prop_juicestand"_hash;
	static const Hash weaponHash     = "weapon_specialcarbine"_hash;
	static const float minDistance   = 2.f;
	static const float maxDistance   = 7.f;
	static const float maxSpeedCheck = 40.f;
	Ped player                       = PLAYER_PED_ID();
	Vector3 pos                      = GET_ENTITY_COORDS(player, false);
	// Make distance to player dependent on players speed
	float playerSpeed                = std::min(std::max(0.f, GET_ENTITY_SPEED(player)), maxSpeedCheck);
	float fixedDistance              = ((playerSpeed / maxSpeedCheck) * (maxDistance - minDistance)) + minDistance;
	Vector3 spawnPos                 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, fixedDistance, 0);
	Object ball                      = CreatePoolProp(ballHash, spawnPos.x, spawnPos.y, spawnPos.z - 0.2, true);
	// Randomize weight
	float weight                     = g_Random.GetRandomFloat(1, 100);
	SET_OBJECT_PHYSICS_PARAMS(ball, weight, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
	// Ball needs to be shot at to be dynamic, otherwise it will be frozen
	Vector3 min, max;
	GET_MODEL_DIMENSIONS(ballHash, &min, &max);
	SHOOT_SINGLE_BULLET_BETWEEN_COORDS(spawnPos.x, spawnPos.y, spawnPos.z + max.z - min.z, spawnPos.x, spawnPos.y,
	                                   spawnPos.z, 0, true, weaponHash, 0, false, true, 0.01f);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
    {
        .Name = "Spawn Orange Ball",
        .Id = "misc_spawn_orange_ball",
        .EffectGroupType = EffectGroupType::SpawnGeneric
    }
);