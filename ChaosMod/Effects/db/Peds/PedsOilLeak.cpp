/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	for (Ped ped : GetAllPeds()) 
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, false) || IS_PED_DEAD_OR_DYING(ped, true)) 
		{
			continue;
		}
		Vector3 playerPos = GET_ENTITY_COORDS(ped, false);
		float groundZ = 0;
		if (GET_GROUND_Z_FOR_3D_COORD(playerPos.x, playerPos.y, playerPos.z, &groundZ, false, false)) 
		{
			ADD_PETROL_DECAL(playerPos.x, playerPos.y, groundZ, 2, 2, 1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_OIL_LEAK, nullptr, nullptr, OnTick);
