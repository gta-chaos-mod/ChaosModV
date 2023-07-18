#include <stdafx.h>

static void OnStart()
{
	bool friendly = g_Random.GetRandomInt(0, 1);

	Hash relationshipGroup;
	if (friendly)
	{
		ADD_RELATIONSHIP_GROUP("_COMPANION_CLONE_FRIENDLY", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, "PLAYER"_hash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, "PLAYER"_hash, relationshipGroup);
	}
	else
	{
		ADD_RELATIONSHIP_GROUP("_COMPANION_CLONE_HOSTILE", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, "PLAYER"_hash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, "PLAYER"_hash, relationshipGroup);
	}

	Ped playerPed = PLAYER_PED_ID();

	Ped ped       = CreatePoolClonePed(playerPed);
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}
	else
	{
		SET_ENTITY_HEADING(ped, GET_ENTITY_HEADING(playerPed));
	}

	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);
	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);

	if (friendly)
	{
		SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));
	}

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	GIVE_WEAPON_TO_PED(ped, GET_SELECTED_PED_WEAPON(playerPed), 9999, true, true);

	SET_PED_ACCURACY(ped, 100);
	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Clone Player",
		.Id = "player_clone"
	}
);