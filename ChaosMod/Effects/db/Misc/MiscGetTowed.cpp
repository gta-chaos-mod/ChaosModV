/*
Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	static const Hash tonyaHash    = GET_HASH_KEY("ig_tonya");
	static const Hash towTruckHash = GET_HASH_KEY("towtruck");
	static Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_TOW_TRUCK_TONYA", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	LoadModel(tonyaHash);
	LoadModel(towTruckHash);

	Vector3 rearBottomLeft, frontTopRight;
	GET_MODEL_DIMENSIONS(towTruckHash, &rearBottomLeft, &frontTopRight);
	float towLength   = frontTopRight.y - rearBottomLeft.y;

	Ped player        = PLAYER_PED_ID();
	Vehicle playerVeh = 0;
	float spawnOffset = towLength / 2;
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		playerVeh   = GET_VEHICLE_PED_IS_IN(player, false);
		Hash vehMod = GET_ENTITY_MODEL(playerVeh);
		GET_MODEL_DIMENSIONS(vehMod, &rearBottomLeft, &frontTopRight);
		float playerVehLength = frontTopRight.y - rearBottomLeft.y;
		spawnOffset           = (towLength / 2) + (playerVehLength / 2) + 1;
	}

	Vector3 spawnPoint = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, spawnOffset, 0);
	Vehicle towTruck =
		CreatePoolVehicle(towTruckHash, spawnPoint.x, spawnPoint.y, spawnPoint.z, GET_ENTITY_HEADING(player));
	SET_VEHICLE_ENGINE_ON(towTruck, true, true, false);
	SET_VEHICLE_FORWARD_SPEED(towTruck, GET_ENTITY_SPEED(player));
	SET_VEHICLE_ON_GROUND_PROPERLY(towTruck, 5);

	Ped tonyaPed = CreatePoolPedInsideVehicle(towTruck, 0, tonyaHash, -1);
	SET_PED_RELATIONSHIP_GROUP_HASH(tonyaPed, relationshipGroup);

	if (playerVeh)
	{
		ATTACH_VEHICLE_TO_TOW_TRUCK(towTruck, playerVeh, true, 0, 0, 0);
		SET_VEHICLE_TOW_TRUCK_ARM_POSITION(towTruck, 1.f);
	}
	else
	{
		SET_PED_INTO_VEHICLE(player, towTruck, 0);
	}
	TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(tonyaPed, towTruck, 404, -1630, 29, 9999.f, 262668, 0.f);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Get Towed",
        .Id = "misc_get_towed",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
    }
);