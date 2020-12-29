#include <stdafx.h>

static std::vector<Ped> frozenPeds;

static bool isFrozen(Ped ped) {
	return std::find(frozenPeds.begin(), frozenPeds.end(), ped) != frozenPeds.end();
}

static void OnTick() {
	const std::vector<Ped> peds = GetAllPeds();
	for (Ped ped : GetAllPeds()) {
		if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(ped) && !isFrozen(ped)) {
			frozenPeds.push_back(ped);
		}

		if (isFrozen(ped)) {
			FREEZE_ENTITY_POSITION(ped, true);
		}
	}
}

static void OnStop() {
	for (Ped ped : frozenPeds) {
		FREEZE_ENTITY_POSITION(ped, false);
	}

	std::vector<Ped>().swap(frozenPeds); // Clear frozenPeds and reallocate
}

static RegisterEffect registerEffect(EFFECT_PEDS_NAILGUNS, nullptr, OnStop, OnTick);