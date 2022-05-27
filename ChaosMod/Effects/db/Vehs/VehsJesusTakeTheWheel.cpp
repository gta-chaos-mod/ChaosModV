/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	// If the player isn't in a vehicle, put him in a pink panto
	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);

		Vehicle veh       = CreatePoolVehicle(GET_HASH_KEY("PANTO"), playerPos.x, playerPos.y, playerPos.z,
		                                      GET_ENTITY_HEADING(PLAYER_PED_ID()));
		SET_VEHICLE_COLOURS(veh, 135, 135);
		SET_PED_INTO_VEHICLE(playerPed, veh, -1);
	}

	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_WHEEL_JESUS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));

	Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	SET_PED_INTO_VEHICLE(playerPed, veh, -2);

	Ped jesus = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
	SET_PED_RELATIONSHIP_GROUP_HASH(jesus, relationshipGroup);

	SET_ENTITY_PROOFS(jesus, true, false, false, false, false, false, false, false);

	// Try to fetch waypoint coords (if waypoint exists)
	Vector3 coords;
	bool found = false;
	if (IS_WAYPOINT_ACTIVE())
	{
		coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		found  = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			Blip blip = GET_FIRST_BLIP_INFO_ID(i);
			if (DOES_BLIP_EXIST(blip))
			{
				coords = GET_BLIP_COORDS(blip);
				found  = true;

				break;
			}
		}
	}

	if (found)
	{
		TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(jesus, veh, coords.x, coords.y, coords.z, 9999.f, 262668, 0.f);
	}
	else
	{
		TASK_VEHICLE_DRIVE_WANDER(jesus, veh, 9999.f, 4176732);
	}

	SET_PED_KEEP_TASK(jesus, true);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(jesus, true);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Jesus Take The Wheel",
		.Id = "veh_jesustakethewheel"
	}
);