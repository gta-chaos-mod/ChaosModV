#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	static const Hash faggioHash = GET_HASH_KEY("FAGGIO");
	SetSurroundingPedsInVehicles(faggioHash, -1);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SCOOTERBROTHERS, OnStart, EffectInfo
	{
		.Name = "Scooter Brothers",
		.Id = "peds_scooterbrothers",
		.EffectGroupType = EEffectGroupType::TrafficSpawner
	}
);