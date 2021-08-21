/*
Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
    static Hash enemyHash = GET_HASH_KEY("u_m_y_rsranger_01");
    static Hash weaponHash = GET_HASH_KEY("weapon_raycarbine");
    Ped ped = CreateHostilePed(enemyHash, weaponHash);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_SPACE_RANGER, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Space Ranger",
        .Id = "peds_spawn_space_ranger",
        .EEffectGroupType = EEffectGroupType::SpawnEnemy
    }
);
