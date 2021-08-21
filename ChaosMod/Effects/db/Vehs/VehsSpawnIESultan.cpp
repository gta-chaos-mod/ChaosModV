#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	float playerHeading = GET_ENTITY_HEADING(playerPed);

	static const Hash sultanModel = GET_HASH_KEY("SULTANRS");

	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Vehicle veh = CreatePoolVehicle(sultanModel, playerPos.x, playerPos.y, playerPos.z, heading);
	SET_VEHICLE_COLOURS(veh, 64, 64);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_IESULTAN", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, relationshipGroup);

	static const Hash model = GET_HASH_KEY("g_m_m_armboss_01");

	static const Hash microSmgHash = GET_HASH_KEY("WEAPON_MICROSMG");

	Ped ped = CreatePoolPedInsideVehicle(veh, 4, model, -1);
	SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
	SET_PED_ACCURACY(ped, 50);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	WAIT(0);

	ped = CreatePoolPedInsideVehicle(veh, 4, model, 0);
	SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
	SET_PED_ACCURACY(ped, 50);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	WAIT(0);
}

static RegisterEffect registerEffect(EFFECT_SPAWN_IE_SULTAN, OnStart, EffectInfo
	{
		.Name = "Spawn Blue Sultan",
		.Id = "spawn_bluesultan",
		.EEffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);