/*
    Effect by Gorakh
*/

#include <stdafx.h>

struct TemporarilyInvincibleEntity
{
	Entity entity;
	int endInvincibilityTick;

	TemporarilyInvincibleEntity(Entity _entity, int waitTime)
	{
		entity               = _entity;
		endInvincibilityTick = GET_GAME_TIMER() + waitTime;
	}
};

static std::vector<TemporarilyInvincibleEntity> temporarilyInvincibleEntities;

static std::vector<Entity> excludeEntities;

static void OnStart()
{
	// Exclude all peds and vehicles that are dead at the start of the effect
	excludeEntities = {};

	for (Ped ped : GetAllPeds())
	{
		if (!DOES_ENTITY_EXIST(ped) || IS_ENTITY_DEAD(ped, 0))
		{
			excludeEntities.push_back(ped);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (!DOES_ENTITY_EXIST(veh) || IS_ENTITY_DEAD(veh, 0))
		{
			excludeEntities.push_back(veh);
		}
	}

	temporarilyInvincibleEntities = {};
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (DOES_ENTITY_EXIST(ped) && IS_ENTITY_DEAD(ped, 0)
		    && std::find(excludeEntities.begin(), excludeEntities.end(), ped) == excludeEntities.end())
		{
			excludeEntities.push_back(ped);
			Ped clone = CreatePoolClonePed(ped);

			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				Vehicle pedVehicle = GET_VEHICLE_PED_IS_IN(ped, false);

				int pedSeatIndex   = -2;

				int maxSeats       = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(pedVehicle));
				for (int i = -1; i < maxSeats; i++)
				{
					if (!IS_VEHICLE_SEAT_FREE(pedVehicle, i, false) && GET_PED_IN_VEHICLE_SEAT(pedVehicle, i, 0) == ped)
					{
						pedSeatIndex = i;
						break;
					}
				}

				// Best way I could find to free up the seat, the ped is dead so TASK_LEAVE_VEHICLE doesn't work, and
				// setting the ped's coordinates to somewhere else left the seat in some kind of weird half-occipied
				// state
				if (!IS_PED_A_PLAYER(ped))
				{
					SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
					DELETE_ENTITY(&ped);

					SET_PED_INTO_VEHICLE(clone, pedVehicle, pedSeatIndex);
				}
			}
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (DOES_ENTITY_EXIST(veh) && IS_ENTITY_DEAD(veh, 0)
		    && std::find(excludeEntities.begin(), excludeEntities.end(), veh) == excludeEntities.end())
		{
			excludeEntities.push_back(veh);
			Vehicle cloneVeh = CreatePoolCloneVehicle(veh);

			int maxSeats     = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(GET_ENTITY_MODEL(veh));
			for (int i = -1; i < maxSeats; i++)
			{
				if (!IS_VEHICLE_SEAT_FREE(veh, i, false))
				{
					SET_PED_INTO_VEHICLE(GET_PED_IN_VEHICLE_SEAT(veh, i, 0), cloneVeh, i);
				}
			}

			if (GET_IS_VEHICLE_ENGINE_RUNNING(veh))
			{
				SET_VEHICLE_ENGINE_ON(cloneVeh, true, true, false);
			}

			// Prevent vehicle from blowing up instantly on spawn, creating an infinite feedback loop of spawning and
			// exploding vehicles
			SET_ENTITY_INVINCIBLE(cloneVeh, true);
			temporarilyInvincibleEntities.push_back(TemporarilyInvincibleEntity(cloneVeh, 500));
		}
	}

	for (auto it = temporarilyInvincibleEntities.begin(); it != temporarilyInvincibleEntities.end();)
	{
		TemporarilyInvincibleEntity invincibleEntity = *it;

		if (!DOES_ENTITY_EXIST(invincibleEntity.entity) || GET_GAME_TIMER() >= invincibleEntity.endInvincibilityTick)
		{
			if (DOES_ENTITY_EXIST(invincibleEntity.entity))
			{
				SET_ENTITY_INVINCIBLE(invincibleEntity.entity, false);
			}

			it = temporarilyInvincibleEntities.erase(it);
		}
		else
		{
			SET_ENTITY_INVINCIBLE(invincibleEntity.entity, true);

			it++;
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Resurrection Day",
		.Id = "misc_clone_on_death",
		.IsTimed = true,
		.IsShortDuration = true
	}
);