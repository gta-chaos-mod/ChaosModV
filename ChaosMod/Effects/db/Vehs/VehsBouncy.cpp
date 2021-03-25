#include <stdafx.h>

/*
	Effect by ProfessorBiddle
*/

static void OnTick()
{
	float velFactor;
	for (auto veh : GetAllVehs())
	{
		SET_ENTITY_INVINCIBLE(veh, true);
		//SET_VEHICLE_FIXED(veh);
		if (HAS_ENTITY_COLLIDED_WITH_ANYTHING(veh))
		{
			Vector3 vel = GET_ENTITY_VELOCITY(veh);
			if ((vel.x < 10) && (vel.y < 10) && (vel.z < 10))
			{
				velFactor = 300.f;
			}
			else
			{
				velFactor = 60.f;
			}
			APPLY_FORCE_TO_ENTITY(veh, 0, vel.x * -velFactor, vel.y * -velFactor, vel.z * -velFactor, .0f, .0f, .0f, 0, true, true, true, false, true);
			
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
static RegisterEffect registerEffect2(EFFECT_VEH_BOUNCY, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Bouncy Vehicles",
		.Id = "veh_bouncy",
		.IsTimed = true
	}
);