/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = 0x3CDCA742;

	Ped playerPed                   = PLAYER_PED_ID();
	Vector3 playerPos               = GET_ENTITY_COORDS(playerPed, false);

	static const Hash playerGroup   = GET_HASH_KEY("PLAYER");
	static const Hash civGroup      = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup   = GET_HASH_KEY("CIVFEMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_MIME", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, femCivGroup);

	Ped ped = CreatePoolPed(4, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f);

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);

	SET_ENTITY_PROOFS(ped, false, true, true, false, false, false, false, false);

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_COMBATSHOTGUN"), 9999, true, true);
	SET_CURRENT_PED_WEAPON(ped, GET_HASH_KEY("WEAPON_COMBATSHOTGUN"), true);
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

	while (GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0) == 0) // doing '!' doesn't work
		WAIT(0);
	
	Entity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0);
	SET_ENTITY_ALPHA(weapon, 0, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Murderous Mime",
		.Id = "spawn_mime",
		.EffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);