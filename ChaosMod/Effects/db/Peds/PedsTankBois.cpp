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

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Tanks A Lot",
        .Id = "peds_tank_bois",
        .EffectGroupType = EEffectGroupType::TrafficSpawner
    }
);