/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	static const float maxDistance = 120.f;
    SetSurroundingPedsInVehicles("rhino"_hash, maxDistance);
}

static RegisterEffect registerEffect(EFFECT_PEDS_TANK_BOIS, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Tanks A Lot",
        .Id = "peds_tank_bois",
        .EffectGroupType = EEffectGroupType::TrafficSpawner
    }
);
