/*
	Effect by Reguas
*/

#include <stdafx.h>
#include <set>


static std::set<Entity> freeEntities;

static void OnStart()
{
	freeEntities.insert(PLAYER_PED_ID());
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Entity touching = player;
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		if (ped != player) {
			entities.push_back(ped);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (!IS_PED_IN_VEHICLE(player, veh, false))
		{
			entities.push_back(veh);
		}
		else
		{
			freeEntities.insert(veh);
			touching = veh;
		}
	}

	for (Entity prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	for (Entity entity : entities)
	{
		if (IS_ENTITY_TOUCHING_ENTITY(touching, entity))
		{
			freeEntities.insert(entity);
		}

		if (freeEntities.contains(entity))
		{
			FREEZE_ENTITY_POSITION(entity, false);
		}
		else
		{
			if (IS_ENTITY_A_PED(entity))
			{
				TASK_PAUSE(entity, 1);
			}
			if (IS_ENTITY_A_VEHICLE(entity))
			{
				// to avoid weird vehicle spawns
				SET_VEHICLE_ON_GROUND_PROPERLY(entity, 5.f);
			}
			FREEZE_ENTITY_POSITION(entity, true);
		}
	}
}

static void OnStop()
{
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		entities.push_back(ped);
	}

	for (Vehicle veh : GetAllVehs())
	{
		entities.push_back(veh);
	}

	for (Entity prop : GetAllProps())
	{
		entities.push_back(prop);
	}

	for (Entity entity : entities)
	{
		FREEZE_ENTITY_POSITION(entity, false);
	}

	freeEntities.clear();
}

static RegisterEffect registerEffect(EFFECT_MISC_FROZEN, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Frozen",
		.Id = "misc_frozen",
		.IsTimed = true,
		.IsShortDuration = true
	}
);
