#include <stdafx.h>

#define ApplyForce(entity, force) APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(entity, 1, 0, 0, force, true, false, true, true)

static void OnStop()
{
	CAM:STOP_GAMEPLAY_CAM_SHAKING(true);
}

static void OnTick()
{
	CAM::SHAKE_GAMEPLAY_CAM("LARGE_EXPLOSION_SHAKE", 0.05);
	auto shook = GET_RANDOM_FLOAT_IN_RANGE(-9.f, 7.f); // low slightly lower than oppisite of upper to decrease chances of stuff going into space.
	for (auto veh : GetAllVehs())
	{
		ApplyForce(veh, shook);
	}
	for (auto prop : GetAllProps())
	{
		ApplyForce(prop, shook);
	}
	for (auto ped : GetAllPeds())
	{
		ApplyForce(ped, shook);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_EARTHQUAKE, nullptr, OnStop, OnTick);