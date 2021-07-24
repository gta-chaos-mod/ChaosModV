/*
	Effect by Marvis
*/

#include <stdafx.h>

static void OnStart()
{

	static constexpr Hash chimpHash = 2825402133;

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_CHIMP", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, femCivGroup);

	Ped ped = CreatePoolPed(4, chimpHash, playerPos.x, playerPos.y, playerPos.z, 0.f);
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 0, true);

	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_STONE_HATCHET"), 0, true, true);
	TASK_COMBAT_PED(ped, playerPed, 0, 16);
}


// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_SPAWN_ANGRY_CHIMP, OnStart, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Spawn Angry Chimp",
		.Id = "spawn_angry_chimp",
		.EEffectGroupType = EEffectGroupType::SpawnEnemy,
	}
);