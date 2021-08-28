#include <stdafx.h>

static void OnTick()
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

	for (auto ped : GetAllPeds())
	{
		Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, false);
		if (IS_HORN_ACTIVE(pedVeh))
		{
			for (Entity entity : entities)
			{
				if (entity != ped && entity != pedVeh)
				{
					static float rangeLimit = 10;
					Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);
					Vector3 pedCoord = GET_ENTITY_COORDS(ped, false);
					float distance = GET_DISTANCE_BETWEEN_COORDS(pedCoord.x, pedCoord.y, pedCoord.z, entityCoord.x, entityCoord.y, entityCoord.z, true);
					if (distance < rangeLimit)
					{
						if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
						{
							SET_PED_TO_RAGDOLL(entity, 2000, 2000, 0, true, true, false);
						}
						APPLY_FORCE_TO_ENTITY(entity, 3, entityCoord.x - pedCoord.x, entityCoord.y - pedCoord.y, entityCoord.z - pedCoord.z, 0, 0, 0, false, false, true, true, false, true);
					}
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_HONK_FIELD, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Honk Field",
		.Id = "vehs_honkfield",
		.IsTimed = true,
		.IsShortDuration = false
	}
);