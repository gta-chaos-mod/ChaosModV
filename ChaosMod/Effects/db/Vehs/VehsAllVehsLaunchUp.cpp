#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		Vector3 vel = GET_ENTITY_VELOCITY(veh);

		SET_ENTITY_VELOCITY(veh, vel.x, vel.y, 100.f);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_LAUNCH, OnStart);