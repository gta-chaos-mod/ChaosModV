#include <stdafx.h>

static void OnStart()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_ENTITY_VELOCITY(veh, .0f, .0f, 10000.f);
	}
}

static RegisterEffect registerEffect(EFFECT_VEHS_LAUNCH, OnStart);