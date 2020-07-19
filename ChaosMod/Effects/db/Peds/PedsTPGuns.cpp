#include <stdafx.h>
/**
 * Effect by: https://github.com/Elias-Graf, modified by pongo1231
 **/

static std::map<Ped, bool> hasShoot;

static void OnStart()
{
	// We want to remove potential old peds
	hasShoot.clear();
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		// Only add ped to map if it doesn't already contain them
		if (!hasShoot.count(ped))
		{
			hasShoot.insert(std::pair<Ped, bool>(ped, false));
		}

		if (IS_PED_SHOOTING(ped))
		{
			std::map<Ped, bool>::iterator it = hasShoot.find(ped);
			if (it != hasShoot.end())
			{
				it->second = true;
			}
		}
	}

	std::map<Ped, bool>::iterator it;
	for (it = hasShoot.begin(); it != hasShoot.end(); it++)
	{
		Ped ped = it->first;
		bool& hasShoot = it->second;
		
		// If this ped has shoot and the bulled impacted
		Vector3 impactCoords;
		if (hasShoot && GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &impactCoords))
		{
			hasShoot = false;

			Entity toTeleport = ped;

			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				toTeleport = GET_VEHICLE_PED_IS_IN(ped, false);
			}

			float heading = GET_ENTITY_HEADING(toTeleport);
			Vector3 vel = GET_ENTITY_VELOCITY(toTeleport);
			SET_ENTITY_COORDS(toTeleport, impactCoords.x, impactCoords.y, impactCoords.z, false, false, false, false);
			SET_ENTITY_VELOCITY(toTeleport, vel.x, vel.y, vel.z);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_PORTAL_GUN, OnStart, nullptr, OnTick);