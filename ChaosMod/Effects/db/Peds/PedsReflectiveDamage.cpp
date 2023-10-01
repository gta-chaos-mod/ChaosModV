/*
	Effect By Rylxnd
*/
#include <stdafx.h>

static std::unordered_map<Ped, int> mappedPeds;

static void OnStop()
{
	mappedPeds.clear();
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (!mappedPeds.contains(ped))
		{
			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
			mappedPeds.emplace(ped, GET_ENTITY_HEALTH(ped));
		}
	}

	for (auto it = mappedPeds.begin(); it != mappedPeds.end();)
	{
		auto ped     = it->first;
		auto &health = it->second;

		if (!DOES_ENTITY_EXIST(ped))
		{
			it = mappedPeds.erase(it);
			continue;
		}

		if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(ped))
		{
			for (Ped attackerPed : GetAllPeds())
			{
				if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, attackerPed, true))
				{
					int damage = health - GET_ENTITY_HEALTH(ped);

					if (damage <= 0)
						break;

					APPLY_DAMAGE_TO_PED(attackerPed, damage, false, false);
					SET_ENTITY_HEALTH(ped, GET_ENTITY_HEALTH(ped), 0);

					health = GET_ENTITY_HEALTH(ped);

					break;
				}
			}

			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}
		else if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_VEHICLE(ped))
		{
			for (Vehicle attackerVehicle : GetAllVehs())
			{
				if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, attackerVehicle, true))
				{
					Ped attackerPed = GET_PED_IN_VEHICLE_SEAT(attackerVehicle, -1, false);
					int damage      = health - GET_ENTITY_HEALTH(ped);

					if (damage <= 0)
						break;

					APPLY_DAMAGE_TO_PED(attackerPed, damage, false, false);
					SET_ENTITY_HEALTH(ped, GET_ENTITY_HEALTH(ped), 0);

					health = GET_ENTITY_HEALTH(ped);

					break;
				}
			}

			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}
		else
		{
			health = GET_ENTITY_HEALTH(ped);
			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}

		if (IS_ENTITY_DEAD(ped, false))
		{
			it = mappedPeds.erase(it);
		}

		it++;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Friendly Fire",
		.Id = "peds_reflectivedamage",
		.IsTimed = true,
		.IsShortDuration = true,
	}
);
