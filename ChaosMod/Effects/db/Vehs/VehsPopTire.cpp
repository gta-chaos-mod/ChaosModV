#include <stdafx.h>
// Not repairing after the effect is intentional. 
static void OnStart()
{
	auto playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		for (int i = 0; i < 48; i++) // using code from VehsPopTires.cpp
		{
			if (Random::GetRandomInt(0, 1)) // random true / false to get ideally 50% of tires popped.
			{
				SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.f);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_POP_TIRE, OnStart);