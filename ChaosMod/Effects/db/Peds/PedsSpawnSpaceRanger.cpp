/*
Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
    static Hash bikerHash = GET_HASH_KEY("u_m_y_rsranger_01");
    static Hash weaponHash = GET_HASH_KEY("weapon_rayminigun");
    Ped ped = CreateHostilePed(bikerHash, weaponHash);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_SPACE_RANGER, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Space Ranger",
        .Id = "peds_spawn_space_ranger",
        .EffectGroupType = EffectGroupType::SPAWN_ENEMY
    }
);
