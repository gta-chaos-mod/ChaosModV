/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Vehicle.h"

static void OnStart()
{
	static const float maxDistance = 120.f;
	SetSurroundingPedsInVehicles("rhino"_hash, maxDistance);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
    {
        .Name = "Tanks A Lot",
        .Id = "peds_tank_bois",
        .EffectGroupType = EffectGroupType::TrafficSpawner
    }
);