/*
Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
	static const float maxDistance = 120.f;
	static const Hash vehHash = GET_HASH_KEY("rhino");
    SetSurroundingPedsInVehicles(vehHash, maxDistance);
}

static RegisterEffect registerEffect(EFFECT_PEDS_TANK_BOIS, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Tanks A Lot",
        .Id = "peds_tank_bois",
        .EEffectGroupType = EEffectGroupType::TrafficSpawner
    }
);
