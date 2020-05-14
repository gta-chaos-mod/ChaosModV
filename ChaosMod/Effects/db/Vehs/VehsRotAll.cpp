#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		float head = GET_ENTITY_HEADING(veh);
		Vector3 vel = GET_ENTITY_VELOCITY(veh);

		if (head < 180.f)
		{
			SET_ENTITY_HEADING(veh, head + 180.f);
		}
		else
		{
			SET_ENTITY_HEADING(veh, head - 180.f);
		}

		SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);
	}
}

static RegisterEffect registerEffect(EFFECT_ROT_ALL_VEHS, OnStart);