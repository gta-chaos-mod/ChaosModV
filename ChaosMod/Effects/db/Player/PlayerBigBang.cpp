#include <stdafx.h>

//Effect by ProfessorBiddle, Based on code from Pongo and Last0xygen

static void OnStart()
{
	std::vector<Entity> entities;
	int maxEntities = 20;

	for (auto ped : GetAllPeds())
	{
		if (maxEntities == 10)
		{
			break;
		}

		

		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
		{
			SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
			entities.push_back(ped);
			maxEntities--;
		}
	}

	auto playerPed = PLAYER_PED_ID();

	auto playerInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	for (auto veh : GetAllVehs())
	{
		if (maxEntities == 0)
		{
			break;
		}

		maxEntities--;

		if ((!playerInVeh || veh != playerVeh) && !IS_ENTITY_A_MISSION_ENTITY(veh))
		{
			SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
			entities.push_back(veh);
		}
	}

	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	for (auto entity : entities)
	{
		SET_ENTITY_COORDS(entity, playerPos.x, playerPos.y, playerPos.z + 2, false, false, false, false);
	}

	WAIT(0);

	for (auto entity : entities)
	{
		SET_ENTITY_AS_MISSION_ENTITY(entity, false, false);
		SET_ENTITY_AS_NO_LONGER_NEEDED(&entity);
	}
	WAIT(200);

	for (Ped ped : GetAllPeds())
	{
		if (ped != playerPed) {
			entities.push_back(ped);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (!IS_PED_IN_VEHICLE(playerPed, veh, false))
		{
			entities.push_back(veh);
		}
	}

	for (Entity prop : GetAllProps())
	{
		entities.push_back(prop);
	}


	for (Entity entity : entities)
	{
		
		Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);
		if (GET_DISTANCE_BETWEEN_COORDS(entityCoord.x, entityCoord.y, entityCoord.z, playerPos.x, playerPos.y, playerPos.z, true) <= 20)
		{
			
			APPLY_FORCE_TO_ENTITY(entity, 1, (entityCoord.x - playerPos.x) * 200, (entityCoord.y - playerPos.y) * 200 , (entityCoord.z - playerPos.z) * 400, 0, 0, 0, 0, false, false, true, false, true);
			
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BIG_BANG, OnStart);