/*
Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Peds.h"

static void OnStart()
{
	Ped ped = CreateHostilePed("u_m_y_rsranger_01"_hash, "weapon_raycarbine"_hash);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Space Ranger",
        .Id = "peds_spawn_space_ranger",
        .EffectGroupType = EEffectGroupType::SpawnEnemy
    }
);