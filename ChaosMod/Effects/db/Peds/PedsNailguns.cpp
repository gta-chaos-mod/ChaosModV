#include <stdafx.h>

static std::vector<Ped> frozenPeds;
Entity nailgun;

static bool isFrozen(Ped ped) {
	return std::find(frozenPeds.begin(), frozenPeds.end(), ped) != frozenPeds.end();
}

static void OnStart() {
	Ped player = PLAYER_PED_ID();
	int bone = GET_PED_BONE_INDEX(player, 0xDEAD); // (kolyaventuri): Right Hand

	nailgun = CREATE_OBJECT(1854391800, 0, 0, 0, false, false, false); // (kolyaventuri): 1854391800 == nailgun hash
	ATTACH_ENTITY_TO_ENTITY(nailgun, player, bone, 0.15f, 0.05f, 0.01f, 70.0f, 0.0f, 180.f, true, true, false, false, 2, true);
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

	DELETE_OBJECT(&nailgun);
	std::vector<Ped>().swap(frozenPeds); // Clear frozenPeds and reallocate
}

static RegisterEffect registerEffect(EFFECT_PEDS_NAILGUNS, OnStart, OnStop, OnTick);