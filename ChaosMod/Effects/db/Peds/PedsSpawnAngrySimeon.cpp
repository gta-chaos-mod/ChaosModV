/*
	Effect by Reguas
*/

#include <stdafx.h>

static void OnStart()
{
	static constexpr Hash modelHashSimeon = 0xC0937202;
	static constexpr Hash modelHashLamar = 0x65B93076;
	static constexpr Hash modelHashFranklin = 0x9B22DBAF;

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_SIMEON", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);

	Ped ped = CreatePoolPed(4, modelHashSimeon, playerPos.x, playerPos.y, playerPos.z, 0.f);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 7);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		BRING_VEHICLE_TO_HALT(veh, 0.1f, 10, 0);
		Ped pedAssistant = CreatePoolPed(4, g_Random.GetRandomInt(0, 1) == 0 ? modelHashLamar : modelHashFranklin, playerPos.x, playerPos.y, playerPos.z, 0.f);

		SET_PED_RELATIONSHIP_GROUP_HASH(pedAssistant, relationshipGroup);
		SET_RAGDOLL_BLOCKING_FLAGS(pedAssistant, 2);

		TASK_VEHICLE_DRIVE_WANDER(pedAssistant, veh, 9999.f, 572);

		SET_PED_KEEP_TASK(pedAssistant, true);		

		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedAssistant, true);

		TASK_PAUSE(ped, 1500);
	}


	REMOVE_ALL_PED_WEAPONS(playerPed, false);

	switch (GET_ENTITY_MODEL(playerPed))
	{
	case 0xD7114C9:
		STAT_SET_INT(GET_HASH_KEY("SP0_TOTAL_CASH"), 0, true);
		break;
	case 0x9B22DBAF:
		STAT_SET_INT(GET_HASH_KEY("SP1_TOTAL_CASH"), 0, true);
		break;
	case 0x9B810FA2:
		STAT_SET_INT(GET_HASH_KEY("SP2_TOTAL_CASH"), 0, true);
		break;
	}
}

static RegisterEffect registerEffect(EFFECT_ANGRY_SIMEON, OnStart, EffectInfo
	{
		.Name = "Legitimate Businessman",
		.Id = "peds_angrysimeon",
		.EEffectGroupType = EEffectGroupType::SpawnEnemy
	}
);