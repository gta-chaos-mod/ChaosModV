#include <stdafx.h>

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

		if ((!IS_PED_IN_ANY_VEHICLE(playerPed, false) || veh != GET_VEHICLE_PED_IS_IN(playerPed, false))
		    && !IS_ENTITY_A_MISSION_ENTITY(veh))
		{
			SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
			entities.push_back(veh);
		}
	}

	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	for (auto entity : entities)
	{
		SET_ENTITY_COORDS(entity, playerPos.x, playerPos.y, playerPos.z, false, false, false, false);
	}

	WAIT(0);

	for (auto entity : entities)
	{
		SET_ENTITY_AS_MISSION_ENTITY(entity, false, false);
		SET_ENTITY_AS_NO_LONGER_NEEDED(&entity);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport Everything To Player",
		.Id = "player_tpeverything"
	}
);