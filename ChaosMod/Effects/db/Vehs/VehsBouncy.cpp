#include <stdafx.h>

/*
	Effect by ProfessorBiddle
*/

static void OnTick()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, true);
		//SET_VEHICLE_FIXED(veh);
		if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(veh))
		{
			Vector3 vel = GET_ENTITY_VELOCITY(veh);
			APPLY_FORCE_TO_ENTITY(veh, 0, vel.x * -60.f, vel.y * -60.f, vel.z * -60.f, .0f, .0f, .0f, 0, true, true, true, false, true);
			
		}
	}


	
}
static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, false);
	}
}
static RegisterEffect registerEffect2(EFFECT_VEH_BOUNCY, nullptr, OnStop, OnTick);
