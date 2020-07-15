#include <stdafx.h>

/**
 * Effect by: https://github.com/Elias-Graf, modified
 **/

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 impactCoords;
			if (GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &impactCoords))
			{
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
}

static RegisterEffect registerEffect(EFFECT_PEDS_PORTAL_GUN, nullptr, nullptr, OnTick);