#include <stdafx.h>

static void OnTick()
{
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);
	for (Vehicle veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			SET_VEHICLE_ENGINE_HEALTH(veh, -1.f);
			SET_VEHICLE_PETROL_TANK_HEALTH(veh, -1.f);
			SET_VEHICLE_BODY_HEALTH(veh, -1.f);
		}
	}

	for (Ped ped : GetAllPeds())
	{
		Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, false);
		if (!IS_PED_A_PLAYER(ped) && !IS_ENTITY_ON_FIRE(ped) && !IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			START_ENTITY_FIRE(ped);
		}
		else if (IS_PED_IN_ANY_VEHICLE(ped, false) && !IS_PED_A_PLAYER(ped) && !IS_PED_IN_VEHICLE(ped, playerVeh, false))
		{
			SET_PED_TO_RAGDOLL(ped, 500, 500, 0, true, true, false);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_GLOBAL_ARSON, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "This Is Fine",
		.Id = "global_arson",
		.IsTimed = true,
		.IsShortDuration = true,
	}
);