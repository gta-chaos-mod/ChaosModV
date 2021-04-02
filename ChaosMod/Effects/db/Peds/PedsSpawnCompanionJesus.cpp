/*
	Effect by Zima2115
*/

#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHash = -835930287;

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_JESUS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, playerGroup, relationshipGroup);

	Ped ped = CreatePoolPed(4, modelHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);
	SET_ENTITY_PROOFS(ped, false, true, true, false, false, false, false, false);

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);

	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);

	SET_PED_ACCURACY(ped, 100);
	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));
}

static RegisterEffect registerEffect(EFFECT_SPAWN_COMPANION_JESUS, OnStart, EffectInfo
	{
		.Name = "Spawn Companion Jesus",
		.Id = "spawn_compjesus",
		.EffectGroupType = EffectGroupType::SPAWN_COMPANION
	}
);