/*
	Effect by ProfessorBiddle
*/

#include <stdafx.h>

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	//delete current vehicle
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		Vector3 vel = GET_ENTITY_VELOCITY(playerVeh);
		Vector3 oldPos = GET_ENTITY_COORDS(playerVeh, false);
		SET_ENTITY_COORDS_NO_OFFSET(playerVeh, oldPos.x, oldPos.y, oldPos.z + 5, 0, 0, 0);
		SET_ENTITY_VELOCITY(playerVeh, vel.x, vel.y, 40.f);
	}

	static constexpr Hash modelHash = -835930287;
	LoadModel(modelHash);

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_WHEEL_JESUS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));

	//spawn bus and set player into bus
	Vector3 spawnPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	Vehicle veh = CreatePoolVehicle(GET_HASH_KEY("BUS"), spawnPos.x, spawnPos.y, spawnPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
	SET_PED_INTO_VEHICLE(playerPed, veh, -2);

	//put jesus behind wheel
	//Ped jesus = CREATE_PED_INSIDE_VEHICLE(veh, 4, modelHash, -1, true, false);
	Ped jesus = CreatePoolPed(4, modelHash, spawnPos.x, spawnPos.y + 50.f, spawnPos.z, -1);
	SET_PED_INTO_VEHICLE(jesus, veh, -1);


	SET_PED_RELATIONSHIP_GROUP_HASH(jesus, relationshipGroup);

	SET_ENTITY_PROOFS(jesus, true, false, false, false, false, false, false, false);

	// Try to fetch waypoint coords (if waypoint exists)
	Vector3 coords;
	bool found = false;
	if (IS_WAYPOINT_ACTIVE())
	{
		coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		found = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			Blip blip = GET_FIRST_BLIP_INFO_ID(i);
			if (DOES_BLIP_EXIST(blip))
			{
				coords = GET_BLIP_COORDS(blip);
				found = true;

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

	//jesus is helpful this time
	//SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(jesus, true);
	
}

static RegisterEffect registerEffect(EFFECT_VEH_BUSJESUS, OnStart);