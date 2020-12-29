#include <stdafx.h>

static std::vector<Ped> frozenPeds;
static std::map<Ped, Entity> pedGuns;

static bool isFrozen(const Ped ped) {
	return std::find(frozenPeds.begin(), frozenPeds.end(), ped) != frozenPeds.end();
}

static bool hasNailgun(const Ped ped) {
	return pedGuns.count(ped) > 0;
}

static void giveNailgun(const Ped ped) {
	int bone = GET_PED_BONE_INDEX(ped, 0xDEAD); // (kolyaventuri): Right Hand

	Entity nailgun = CREATE_OBJECT(1854391800, 0, 0, 0, false, false, false); // (kolyaventuri): 1854391800 == nailgun hash
	ATTACH_ENTITY_TO_ENTITY(nailgun, ped, bone, 0.15f, 0.05f, 0.01f, 70.0f, 0.0f, 180.f, true, true, false, false, 2, true);
	pedGuns[ped] = nailgun;
}

static void OnTick() {
	const std::vector<Ped> peds = GetAllPeds();

	for (const Ped ped : GetAllPeds()) {
		if (!hasNailgun(ped)) {
			giveNailgun(ped);
		} else {
			Entity nailgun = pedGuns[ped];

			// (kolyaventuri): Check for weapon visiblity
			Weapon weapon = GET_SELECTED_PED_WEAPON(ped);
			int weaponType = GET_WEAPON_DAMAGE_TYPE(weapon);

			int isHoldingGun = weaponType == 3;
			SET_ENTITY_VISIBLE(nailgun, isHoldingGun, 0);
		}

		if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(ped) && !isFrozen(ped)) {
			frozenPeds.push_back(ped);
		}

		if (isFrozen(ped)) {
			FREEZE_ENTITY_POSITION(ped, true);
		}
	}
}

static void OnStop() {
	for (const Ped ped : frozenPeds) {
		FREEZE_ENTITY_POSITION(ped, false);
	}

	for (std::map<Ped, Entity>::iterator it = pedGuns.begin(); it != pedGuns.end(); ++it) {
		Entity nailgun = it->second;

		SET_OBJECT_AS_NO_LONGER_NEEDED(&nailgun);
		DELETE_OBJECT(&nailgun);
	}
	std::vector<Ped>().swap(frozenPeds); // Clear frozenPeds and reallocate
}

static RegisterEffect registerEffect(EFFECT_PEDS_NAILGUNS, nullptr, OnStop, OnTick);