#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Vehicle.h"

static void OnStart()
{
	static const Hash faggioHash = "FAGGIO"_hash;
	SetSurroundingPedsInVehicles(faggioHash, -1);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Scooter Brothers",
		.Id = "peds_scooterbrothers",
		.EffectGroupType = EffectGroupType::TrafficSpawner
	}
);