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

		maxEntities--;

		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
		{
			SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
			entities.push_back(ped);
		}
	}

	auto playerPed = PLAYER_PED_ID();

	for (auto veh : GetAllVehs())
	{
		if (maxEntities == 0)
		{
			break;
		}

		maxEntities--;

		if ((!IS_PED_IN_ANY_VEHICLE(playerPed, false) || veh != GET_VEHICLE_PED_IS_IN(playerPed, false)) && !IS_ENTITY_A_MISSION_ENTITY(veh))
		{
			SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
			entities.push_back(veh);
		}
	}

	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	for (auto entity : entities)
	{
		SET_ENTITY_COORDS(entity, playerPos.x, playerPos.y + 2, playerPos.z, false, false, false, false);
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
		//boom
		for (int i = 0; i < 30; i++)
		{
			APPLY_FORCE_TO_ENTITY(entity, 1, entityCoord.x - playerPos.x, entityCoord.y - playerPos.y, entityCoord.z - playerPos.z, 0, 0, 0, 0, false, false, true, false, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BIG_BANG, OnStart);