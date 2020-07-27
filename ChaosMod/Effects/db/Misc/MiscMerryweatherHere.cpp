/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static Vector3 getRandomOffsetCoord(Vector3 startCoord, float maxOffset)
{
	Vector3 randomCoord;
	randomCoord.x = startCoord.x + g_random.GetRandomInt(-maxOffset, maxOffset);
	randomCoord.y = startCoord.y + g_random.GetRandomInt(-maxOffset, maxOffset);
	randomCoord.z = startCoord.z;
	return randomCoord;
}

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	Hash buzzardHash = GET_HASH_KEY("BUZZARD");
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Vector3 spawnPoint = getRandomOffsetCoord(playerPos, 50);
	Vehicle veh = CreatePoolVehicle(buzzardHash, spawnPoint.x, spawnPoint.y, spawnPoint.z + 50, 0);

	SET_VEHICLE_COLOURS(veh, 0, 0);
	SET_VEHICLE_ENGINE_ON(veh, true, true, true);
	SET_VEHICLE_FORWARD_SPEED(veh, 0); // Needed, so the heli doesn't fall down instantly

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_MERRYWEATHER", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, relationshipGroup);

	static const Hash model = GET_HASH_KEY("csb_mweather");
	static const Hash microSmgHash = GET_HASH_KEY("WEAPON_MICROSMG");

	for (int seatPos = -1; seatPos < 3; seatPos++) {
		Ped ped = CreatePoolPedInsideVehicle(veh, -1, model, seatPos);
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

		SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
		SET_PED_HEARING_RANGE(ped, 9999.f);

		GIVE_WEAPON_TO_PED(ped, microSmgHash, 9999, true, true);
		SET_PED_ACCURACY(ped, 50);

		TASK_COMBAT_PED(ped, playerPed, 0, 16);

		WAIT(0);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_MERRYWEATHER_HERE, OnStart);
