/*
Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
    static Hash bikerHash = GET_HASH_KEY("G_M_Y_Lost_03");
    static Hash weaponHash = GET_HASH_KEY("weapon_dbshotgun");
    Ped ped = CreateHostilePed(bikerHash, weaponHash);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SPAWN_BIKER, OnStart, EffectInfo
    {
        .Name = "Spawn Biker",
        .Id = "peds_spawn_biker"
    }
);
