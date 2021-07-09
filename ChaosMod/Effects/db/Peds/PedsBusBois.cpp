#include <stdafx.h>

//effect by ProfessorBiddle

static void OnStart()
{
	//not sure exactly what to make this, 60 seemed to work ok
	static const float maxDistance = 60.f;

	static const Hash busHash = GET_HASH_KEY("BUS");

	SetSurroundingPedsInVehicles(busHash, maxDistance);
}

static RegisterEffect registerEffect(EFFECT_PEDS_BUSBOIS, OnStart, EffectInfo
	{
		.Name = "Bus Bois",
		.Id = "peds_busbois",
		.EffectGroupType = EffectGroupType::TRAFFIC_SPAWNER
	}
);