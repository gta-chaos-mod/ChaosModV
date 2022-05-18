/*
Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Peds.h"

static void OnStart()
{
    Ped ped = CreateHostilePed("u_m_y_juggernaut_01"_hash, "weapon_minigun"_hash);
    SET_PED_ARMOUR(ped, 250);
    SET_PED_ACCURACY(ped, 3);
}

static RegisterEffect registerEffect(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Spawn Juggernaut",
        .Id = "peds_spawn_juggernaut",
        .EffectGroupType = EEffectGroupType::SpawnEnemy
    }
);
