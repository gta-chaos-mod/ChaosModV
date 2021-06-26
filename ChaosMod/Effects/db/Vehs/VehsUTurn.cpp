#include <stdafx.h>

/*
 * Effect by juhana
 */

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		Vector3 rot = GET_ENTITY_ROTATION(veh, 2);
		Vector3 vel = GET_ENTITY_VELOCITY(veh);

		SET_ENTITY_ROTATION(veh, -rot.x, -rot.y, rot.z + 180.f, 2, true);
		SET_ENTITY_VELOCITY(veh, -vel.x, -vel.y, -vel.z);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_UTURN, OnStart, EffectInfo
	{
		.Name = "U-Turn",
		.Id = "vehs_uturn"
	}
);