//effect by ProfessorBiddle

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	//not sure exactly what to make this, 60 seemed to work ok
	// now it's 120 because why not
	static const float maxDistance = 120.f;

	SetSurroundingPedsInVehicles("bus"_hash, maxDistance);
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Bus Bois",
		.Id = "peds_busbois",
		.EffectGroupType = EEffectGroupType::TrafficSpawner
	}
);