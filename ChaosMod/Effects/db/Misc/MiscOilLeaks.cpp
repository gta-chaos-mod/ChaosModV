/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	std::vector<Entity> entities;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false) && IS_PED_HUMAN(ped) && !IS_PED_SWIMMING(ped) && !IS_PED_SWIMMING_UNDER_WATER(ped) && !IS_PED_DEAD_OR_DYING(ped, true))
		{
			entities.push_back(ped);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		Hash model = GET_ENTITY_MODEL(veh);
		if (GET_IS_VEHICLE_ENGINE_RUNNING(veh) && !IS_THIS_MODEL_A_HELI(model) && !IS_THIS_MODEL_A_PLANE(model) && !IS_THIS_MODEL_A_BOAT(model))
		{
			entities.push_back(veh);
		}
	}

	int count = 25;
	for (Entity entity : entities)
	{
		if (!DOES_ENTITY_EXIST(entity)) 
		{
			continue;
		}
		Vector3 entityPos = GET_ENTITY_COORDS(entity, false);
		float groundZ = 0;
		if (GET_GROUND_Z_FOR_3D_COORD(entityPos.x, entityPos.y, entityPos.z, &groundZ, false, false))
		{
			ADD_PETROL_DECAL(entityPos.x, entityPos.y, groundZ, 2, 2, 1);
		}

		if (--count == 0)
		{
			count = 25;

			WAIT(0);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_OIL_LEAKS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Oil Trails",
		.Id = "misc_oilleaks",
		.IsTimed = true
	}
);