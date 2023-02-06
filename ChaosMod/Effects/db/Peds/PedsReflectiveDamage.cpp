#include <stdafx.h>

static std::map<Ped, int> mappedPeds;

static void ReplaceMapValue(Ped key, int value)
{
	std::map<Ped, int>::iterator itr;

	itr = mappedPeds.find(key);

	if (itr != mappedPeds.end())
		itr->second = value;
}

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

	for (auto &[ped, health] : mappedPeds)
	{
		if (!DOES_ENTITY_EXIST(ped))
		{
			mappedPeds.erase(ped);
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

					ReplaceMapValue(ped, GET_ENTITY_HEALTH(ped));

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

					ReplaceMapValue(ped, GET_ENTITY_HEALTH(ped));

					break;
				}
			}

			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}
		else
		{
			ReplaceMapValue(ped, GET_ENTITY_HEALTH(ped));
			CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
		}

		if (IS_ENTITY_DEAD(ped, false))
		{
			mappedPeds.erase(ped);
		}
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
