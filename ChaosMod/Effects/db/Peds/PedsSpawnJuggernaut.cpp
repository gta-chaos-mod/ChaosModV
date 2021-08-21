/*
Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
    static Hash enemyHash = GET_HASH_KEY("u_m_y_juggernaut_01");
    static Hash weaponHash = GET_HASH_KEY("weapon_minigun");
    Ped ped = CreateHostilePed(enemyHash, weaponHash);
    SET_PED_ARMOUR(ped, 250);
    SET_PED_ACCURACY(ped, 3);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_JUGGERNAUT, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Juggernaut",
        .Id = "peds_spawn_juggernaut",
        .EEffectGroupType = EEffectGroupType::SpawnEnemy
    }
);
