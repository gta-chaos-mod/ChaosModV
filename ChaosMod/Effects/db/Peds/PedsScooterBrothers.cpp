#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	static const Hash faggioHash = GET_HASH_KEY("FAGGIO");
	SetSurroundingPedsInVehicles(faggioHash, -1);
}

// clang-format off
static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Scooter Brothers",
		.Id = "peds_scooterbrothers",
		.EffectGroupType = EEffectGroupType::TrafficSpawner
	}
);