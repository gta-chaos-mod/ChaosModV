#include "stdafx.h"

Hash ped = 0x705E61F2;

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_BEAST", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);

	Vector3 pos = GET_ENTITY_COORDS(PLAYER_PED_ID(), 1);
	Ped playerPed = PLAYER_PED_ID();
	Ped pd = CreatePoolPed(4, ped, pos.x, pos.y, pos.z, 0.f);

	//Set components
	SET_PED_COMPONENT_VARIATION(pd, 0, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 1, 59, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 2, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 3, 97, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 4, 1, 5, 1);
	SET_PED_COMPONENT_VARIATION(pd, 5, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 6, 0, 10, 1);
	SET_PED_COMPONENT_VARIATION(pd, 7, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 8, 15, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 9, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 10, 0, 0, 1);
	SET_PED_COMPONENT_VARIATION(pd, 11, 79, 0, 1);

	//Set relationship and other stuff
	SET_PED_RELATIONSHIP_GROUP_HASH(pd, relationshipGroup);
	SET_PED_HEARING_RANGE(pd, 9999.f);
	SET_PED_CONFIG_FLAG(pd, 281, true);
	SET_PED_MOVE_RATE_OVERRIDE(pd, 8.f);

	SET_ENTITY_PROOFS(pd, false, true, true, false, false, false, false, false);
	SET_ENTITY_HEALTH(ped, 1000, 0);
	SET_PED_ARMOUR(ped, 1000);

	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(4);

	SET_PED_COMBAT_ATTRIBUTES(pd, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(pd, 46, true);

	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(pd, false);
	SET_RAGDOLL_BLOCKING_FLAGS(pd, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(pd, false);

	GIVE_WEAPON_TO_PED(pd, GET_HASH_KEY("WEAPON_UNARMED"), 9999, true, true);
	TASK_COMBAT_PED(pd, playerPed, 0, 16);

	SET_PED_FIRING_PATTERN(pd, 0xC6EE6B4C);

	while (DOES_ENTITY_EXIST(pd))
	{
		if (IS_ENTITY_DEAD(pd, 1))
		{
			break;
		}
		WAIT(0);
	}
	RESET_AI_MELEE_WEAPON_DAMAGE_MODIFIER();
}

static RegisterEffect reg(OnStart, EffectInfo
	{
		.Name = "Spawn Beast",
		.Id = "peds_spawn_beast"
	}
);
