/*
	Effect by ubscal, modified from Jesus take the wheel
*/

#include <stdafx.h>

static void OnStart()
{
	static const Hash carModel = GET_HASH_KEY("BJXL");
	static constexpr Hash franklinModelHash = -1692214353;

	LoadModel(franklinModelHash);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	float heading = GET_ENTITY_HEADING(playerPed);

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_WHEEL_FRANKLIN", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));

	Vehicle vehicle;
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		vehicle = GET_VEHICLE_PED_IS_IN(playerPed, false);
	}
	else
	{
		vehicle = CreatePoolVehicle(carModel, playerPos.x, playerPos.y, playerPos.z, heading);
		SET_VEHICLE_COLOURS(vehicle, 88, 0);
		SET_VEHICLE_ENGINE_ON(vehicle, true, true, false);
	}

	SET_PED_INTO_VEHICLE(playerPed, vehicle, -2);

	Ped FranklinDrive = CREATE_PED_INSIDE_VEHICLE(vehicle, 4, franklinModelHash, -1, true, false);
	SET_PED_RELATIONSHIP_GROUP_HASH(FranklinDrive, relationshipGroup);
	SET_ENTITY_PROOFS(FranklinDrive, true, false, false, false, false, false, false, false);
	
	Vector3 coords;
	coords.x = -52;
	coords.y = -1106.88;
	coords.z = 26;
	// Probably a really bad way of doing this, but im lazy as fuck.
	TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(FranklinDrive, vehicle, coords.x, coords.y, coords.z, 9999.f, 262668, 0.f);

	SET_PED_KEEP_TASK(FranklinDrive, true);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(FranklinDrive, true);

	// Spawn additional franklins
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier - 1; i++)
	{
		Vehicle veh = CreatePoolVehicle(carModel, playerPos.x, playerPos.y, playerPos.z + ((i + 1) * 2), heading);
		SET_VEHICLE_COLOURS(veh, 88, 0);
		SET_VEHICLE_ENGINE_ON(veh, true, true, false);

		LoadModel(franklinModelHash);
		Ped newPed = CREATE_PED_INSIDE_VEHICLE(veh, 4, franklinModelHash, -1, true, false);

		SET_PED_RELATIONSHIP_GROUP_HASH(newPed, relationshipGroup);

		SET_ENTITY_PROOFS(newPed, true, false, false, false, false, false, false, false);

		// Make additional franklins follow the first one (the one in the players vehicle)
		TASK_VEHICLE_MISSION_PED_TARGET(newPed, veh, FranklinDrive, 7, 9999.f, 4176732, .0f, .0f, false);

		SET_PED_KEEP_TASK(newPed, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(newPed, true);
	}
}

static RegisterEffect registerEffect(EFFECT_REPOSSESSION, OnStart, EffectInfo
	{
		.Name = "Repossession",
		.Id = "veh_repossession",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric,
	}
);
