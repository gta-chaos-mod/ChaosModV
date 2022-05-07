#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_JESUS", &relationshipGroup);
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

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
}

static RegisterEffect registerEffect(EFFECT_ANGRY_JESUS, OnStart, EffectInfo
	{
		.Name = "Spawn Griefer Jesus",
		.Id = "spawn_grieferjesus",
		.EEffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);