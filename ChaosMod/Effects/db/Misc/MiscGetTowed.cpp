/*
Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
	static const Hash tonyaHash = GET_HASH_KEY("ig_tonya");
	static const Hash towTruckHash = GET_HASH_KEY("towtruck");
	static Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_TOW_TRUCK_TONYA", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	LoadModel(tonyaHash);
	LoadModel(towTruckHash);

	Vector3 rearBottomLeft, frontTopRight;
	GET_MODEL_DIMENSIONS(towTruckHash, &rearBottomLeft, &frontTopRight);
	float towLength = frontTopRight.y - rearBottomLeft.y;

	Ped player = PLAYER_PED_ID();
	Vehicle playerVeh = 0;
	float spawnOffset = towLength / 2;
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		Hash vehMod = GET_ENTITY_MODEL(playerVeh);
		GET_MODEL_DIMENSIONS(vehMod, &rearBottomLeft, &frontTopRight);
		float playerVehLength = frontTopRight.y - rearBottomLeft.y;
		spawnOffset = (towLength / 2) + (playerVehLength / 2) + 1;
	}

	Entity relativeEntity = player;
	Vehicle vehicleToTow = playerVeh;
	Vehicle frontTowTruck = 0;
	Ped frontTowTruckTonya = 0;
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		if (vehicleToTow)
		{
			Hash vehModel = GET_ENTITY_MODEL(vehicleToTow);
			GET_MODEL_DIMENSIONS(vehModel, &rearBottomLeft, &frontTopRight);
			float vehicleToTowLength = frontTopRight.y - rearBottomLeft.y;
			spawnOffset = (towLength / 2) + (vehicleToTowLength / 2) + 1;
		}

		Vector3 spawnPoint = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(relativeEntity, 0, spawnOffset, 0);
		frontTowTruck = CreatePoolVehicle(towTruckHash, spawnPoint.x, spawnPoint.y, spawnPoint.z, GET_ENTITY_HEADING(relativeEntity));
		SET_VEHICLE_ENGINE_ON(frontTowTruck, true, true, false);
		SET_VEHICLE_FORWARD_SPEED(frontTowTruck, GET_ENTITY_SPEED(relativeEntity));
		SET_VEHICLE_ON_GROUND_PROPERLY(frontTowTruck, 5);
		SET_VEHICLE_TOW_TRUCK_ARM_POSITION(frontTowTruck, 1.f);

		frontTowTruckTonya = CreatePoolPedInsideVehicle(frontTowTruck, 0, tonyaHash, -1);
		SET_PED_RELATIONSHIP_GROUP_HASH(frontTowTruckTonya, relationshipGroup);
		SET_PED_COMBAT_ATTRIBUTES(frontTowTruckTonya, 3, false); // Don't allow them to leave vehicle by themselves
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(frontTowTruckTonya, true);

		if (vehicleToTow)
		{
			// Somehow, this *sometimes* makes the tow truck attach to the rear of the target vehicle, and I have no idea why that would happen
			ATTACH_VEHICLE_TO_TOW_TRUCK(frontTowTruck, vehicleToTow, false, 0, 0, 0);
		}

		vehicleToTow = frontTowTruck;
		relativeEntity = frontTowTruck;
	}

	// Always place the player in the front tow truck if they don't have a vehicle
	if (!playerVeh)
	{
		SET_PED_INTO_VEHICLE(player, frontTowTruck, 0);
	}

	TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(frontTowTruckTonya, frontTowTruck, 404, -1630, 29, 9999.f, 262668, 0.f);
}

static RegisterEffect registerEffect(EFFECT_MISC_GET_TOWED, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Get Towed",
        .Id = "misc_get_towed",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
    }
);
