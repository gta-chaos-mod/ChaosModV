#include <stdafx.h>

static void OnStart()
{
	static const Hash modelHash = GET_HASH_KEY("a_c_chimp");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CHIMP", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CreatePoolPed(28, modelHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	SET_PED_ACCURACY(ped, 100);
	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_CARBINERIFLE"), 9999, false, true);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Companion Chimp",
		.Id = "spawn_chimp",
		.EffectGroupType = EEffectGroupType::SpawnCompanion
	}
);