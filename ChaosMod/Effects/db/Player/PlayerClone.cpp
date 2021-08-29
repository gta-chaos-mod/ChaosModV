#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	Hash friendlyRelationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CLONE_FRIENDLY", &friendlyRelationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, friendlyRelationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), friendlyRelationshipGroup);

	Hash hostileRelationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CLONE_HOSTILE", &hostileRelationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, hostileRelationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, GET_HASH_KEY("PLAYER"), hostileRelationshipGroup);

	// Make allied and hostile clones fight each other
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, hostileRelationshipGroup, friendlyRelationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, friendlyRelationshipGroup, hostileRelationshipGroup);

	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		bool friendly = g_Random.GetRandomInt(0, 1);

		Hash relationshipGroup = friendly ? friendlyRelationshipGroup : hostileRelationshipGroup;

		Ped ped = CLONE_PED(playerPed, GET_ENTITY_HEADING(playerPed), true, false);
		if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
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
}

static RegisterEffect registerEffect(EFFECT_CLONE_PLAYER, OnStart, EffectInfo
	{
		.Name = "Clone Player",
		.Id = "player_clone"
	}
);