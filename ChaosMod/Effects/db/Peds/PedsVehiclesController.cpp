#include "stdafx.h"

static void OnStartScooters()
{
	static const Hash faggioHash = GET_HASH_KEY("FAGGIO");
	SetSurroundingPedsInVehicles(faggioHash, -1);
}

static RegisterEffect registerEffect(EFFECT_PEDS_SCOOTERBROTHERS, OnStartScooters, EffectInfo //by pongo1231
	{
		.Name = "Scooter Brothers",
		.Id = "peds_scooterbrothers",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);

static void OnStartBuses()
{
	static const Hash busHash = GET_HASH_KEY("BUS");
	SetSurroundingPedsInVehicles(busHash, 120.f);
}

static RegisterEffect registerEffect2(EFFECT_PEDS_BUSBOIS, OnStartBuses, EffectInfo //by ProfessorBiddle
	{
		.Name = "Bus Bois",
		.Id = "peds_busbois",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);

static void OnStartTanks()
{
	static const Hash tankHash = GET_HASH_KEY("RHINO");
	SetSurroundingPedsInVehicles(tankHash, 120.f);
}

static RegisterEffect registerEffect3(EFFECT_PEDS_TANK_BOIS, OnStartTanks, nullptr, nullptr, EffectInfo //by Last0xygen
	{
		.Name = "Tanks A Lot",
		.Id = "peds_tank_bois",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);

static void OnStartTrains()
{
	static const Hash trainHash = GET_HASH_KEY("FREIGHT");
	SetSurroundingPedsInVehicles(trainHash, -1);
}

static RegisterEffect registerEffect4(EFFECT_PEDS_TRAINS, OnStartTrains, EffectInfo //by OnlyRealNubs
	{
		.Name = "Freight Friends",
		.Id = "peds_trains",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);

static void OnStartDozers()
{
	static const Hash dozerHash = GET_HASH_KEY("BULLDOZER");
	SetSurroundingPedsInVehicles(dozerHash, -1);
}

static RegisterEffect registerEffect5(EFFECT_PEDS_DOZERS, OnStartDozers, EffectInfo //by OnlyRealNubs
	{
		.Name = "Dozer Dudes",
		.Id = "peds_dozers",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);

static void OnStartMowers()
{
	static const Hash mowerHash = GET_HASH_KEY("MOWER");
	SetSurroundingPedsInVehicles(mowerHash, -1);
}

static RegisterEffect registerEffect6(EFFECT_PEDS_MOWERMATES, OnStartMowers, EffectInfo //by ubscal, modified
	{
		.Name = "Mower Mates",
		.Id = "peds_mowermates",
		.EEffectGroupType = EEffectGroupType::TrafficSpawner
	}
);