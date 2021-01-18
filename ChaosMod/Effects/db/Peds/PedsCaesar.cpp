#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static const int NUM = 6; // Historically this should be 60 but that seems a bit much
static const float variance = 0.005f;

static void OnStart() {
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup = GET_HASH_KEY("CIVMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_RANDOM", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);

	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, false);

	for (int i = 0; i < NUM; i++) {
		Vector3 newPos = pos;
		newPos.x *= g_random.GetRandomFloat(1.f - variance, 1.f + variance);
		newPos.y *= g_random.GetRandomFloat(1.f - variance, 1.f + variance);

		Ped ped = CreateRandomPoolPed(newPos.x, newPos.y, newPos.z, GET_ENTITY_HEADING(player));
		if (IS_PED_IN_ANY_VEHICLE(player, false))
		{
			SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(player, false), -2);
		}

		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);
		SET_PED_CONFIG_FLAG(ped, 281, true);

		SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

		GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_KNIFE"), 1, false, true);
		SET_PED_ACCURACY(ped, 30);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_CAESAR, OnStart);