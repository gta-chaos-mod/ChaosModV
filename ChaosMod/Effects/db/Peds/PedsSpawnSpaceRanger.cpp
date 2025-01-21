/*
Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Peds.h"

static void OnStart()
{
	Ped ped = CreateHostilePed("u_m_y_rsranger_01"_hash, "weapon_raycarbine"_hash);
	CurrentEffect::SetEffectSoundPlayOptions({ .PlayType = EffectSoundPlayType::FollowEntity, .Entity = ped });
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
    {
        .Name = "Spawn Space Ranger",
        .Id = "peds_spawn_space_ranger",
        .EffectGroupType = EffectGroupType::SpawnEnemy
    }
);