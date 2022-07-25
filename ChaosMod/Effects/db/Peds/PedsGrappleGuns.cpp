/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		CLEAR_ENTITY_LAST_WEAPON_DAMAGE(ped);
	}
	
	for (Vehicle veh : GetAllVehs())
	{
		CLEAR_ENTITY_LAST_WEAPON_DAMAGE(veh);
	}

	for (Entity prop : GetAllProps())
	{
		CLEAR_ENTITY_LAST_WEAPON_DAMAGE(prop);
	}
}

static Entity GetLastEntityShotBy(Ped attacker)
{
	Hash weaponHash;
	GET_CURRENT_PED_WEAPON(attacker, &weaponHash, true);

	for (Ped ped : GetAllPeds())
	{
		if (ped != attacker && HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, attacker, 1) && HAS_ENTITY_BEEN_DAMAGED_BY_WEAPON(ped, weaponHash, 0))
		{
			CLEAR_ENTITY_LAST_WEAPON_DAMAGE(ped);

			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				Vehicle pedVehicle = GET_VEHICLE_PED_IS_IN(ped, false);
				CLEAR_ENTITY_LAST_WEAPON_DAMAGE(pedVehicle);
				return pedVehicle;
			}

			return ped;
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(veh, attacker, 1) && HAS_ENTITY_BEEN_DAMAGED_BY_WEAPON(veh, weaponHash, 0))
		{
			CLEAR_ENTITY_LAST_WEAPON_DAMAGE(veh);
			return veh;
		}
	}

	for (Entity prop : GetAllProps())
	{
		if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(prop, attacker, 1) && HAS_ENTITY_BEEN_DAMAGED_BY_WEAPON(prop, weaponHash, 0))
		{
			SET_ENTITY_DYNAMIC(prop, true);

			CLEAR_ENTITY_LAST_WEAPON_DAMAGE(prop);
			return prop;
		}
	}

	return 0;
}

static void OnTick()
{
	Vector3 impactCoords;

	for (Ped ped : GetAllPeds())
	{
		if (DOES_ENTITY_EXIST(ped) && !IS_ENTITY_DEAD(ped, 0) && GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &impactCoords))
		{
			Entity entity = GetLastEntityShotBy(ped);
			if (entity && DOES_ENTITY_EXIST(entity))
			{
				if (IS_ENTITY_A_PED(entity) && !IS_PED_A_PLAYER(entity) && !IS_PED_IN_ANY_VEHICLE(entity, false))
				{
					SET_PED_TO_RAGDOLL(entity, 500, 500, 0, false, false, false);
				}

				Vector3 entityVelocity = GET_ENTITY_VELOCITY(entity);

				Vector3 addVelocity = (GET_ENTITY_COORDS(ped, true) - impactCoords);
				addVelocity = addVelocity / addVelocity.Length(); // Normalize vector
				addVelocity = addVelocity * 25.f;

				Vector3 newVelocity    = entityVelocity + addVelocity;
				// Apply Force with flag 3 to break trafficlights and other things from their fixed structure
				Memory::ApplyForceToEntity(entity, 3, newVelocity.x, newVelocity.y, newVelocity.z, 0, 0, 0, false,
				                           false, true, true, false, true);
				SET_ENTITY_VELOCITY(entity, newVelocity.x, newVelocity.y, newVelocity.z);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Gravity Guns",
		.Id = "peds_grapple_guns",
		.IsTimed = true
	}
);
