#include <stdafx.h>

static void OnStop()
{
	CAM:STOP_GAMEPLAY_CAM_SHAKING(true);
}

static void OnTick()
{
	CAM::SHAKE_GAMEPLAY_CAM("LARGE_EXPLOSION_SHAKE", 0.05);
	auto playerPed = PLAYER_PED_ID();
	auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	for (auto veh : GetAllVehs())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 1, 0, 0, GET_RANDOM_FLOAT_IN_RANGE(-1500.f, 1500.f), true, false, false, true);
	}
	for (auto prop : GetAllProps())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(prop, 1, 0, 0, GET_RANDOM_FLOAT_IN_RANGE(-750.f, 500.f), true, false, false, true);
	}
	for (auto	 ped : GetAllPeds())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ped, 1, 0, 0, GET_RANDOM_FLOAT_IN_RANGE(-1000.f, 750.f), true, false, false, true);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_EARTHQUAKE, nullptr, OnStop, OnTick);