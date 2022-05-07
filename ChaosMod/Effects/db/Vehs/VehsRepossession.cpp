/*
	Effect by ubscal, modified from Jesus take the wheel
*/

#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		static constexpr Hash modelHash = -1692214353;
		LoadModel(modelHash);

		Hash relationshipGroup;
		ADD_RELATIONSHIP_GROUP("_WHEEL_FRANKLIN", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));

		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_PED_INTO_VEHICLE(playerPed, veh, -2);

		Ped FranklinDrive = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(FranklinDrive, relationshipGroup);

		SET_ENTITY_PROOFS(FranklinDrive, true, false, false, false, false, false, false, false);

		
		Vector3 coords;
		coords.x = -52;
		coords.y = -1106.88;
		coords.z = 26;
		// Probably a really bad way of doing this, but im lazy as fuck.
		TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(FranklinDrive, veh, coords.x, coords.y, coords.z, 9999.f, 262668, 0.f);

		SET_PED_KEEP_TASK(FranklinDrive, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(FranklinDrive, true);
	}
	else
	{

		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
		float heading = GET_ENTITY_HEADING(playerPed);
		static const Hash carModel = GET_HASH_KEY("BJXL");
		Vehicle veh = CreatePoolVehicle(carModel, playerPos.x, playerPos.y, playerPos.z, heading);
		SET_VEHICLE_COLOURS(veh, 88, 0);
		SET_VEHICLE_ENGINE_ON(veh, true, true, false);
		static constexpr Hash modelHash = -1692214353;
		LoadModel(modelHash);

		Hash relationshipGroup;
		ADD_RELATIONSHIP_GROUP("_WHEEL_FRANKLIN", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
		SET_PED_INTO_VEHICLE(playerPed, veh, -2);

		Ped FranklinDrive = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(FranklinDrive, relationshipGroup);

		SET_ENTITY_PROOFS(FranklinDrive, true, false, false, false, false, false, false, false);


		Vector3 coords;
		coords.x = -52;
		coords.y = -1106.88;
		coords.z = 26;
		// Probably a really bad way of doing this, but im lazy as fuck.
		TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(FranklinDrive, veh, coords.x, coords.y, coords.z, 9999.f, 262668, 0.f);

		SET_PED_KEEP_TASK(FranklinDrive, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(FranklinDrive, true);
	}
		
}

static RegisterEffect registerEffect(EFFECT_REPOSSESSION, OnStart, EffectInfo
	{
		.Name = "Repossession",
		.Id = "veh_repossession",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric,
	}
);
