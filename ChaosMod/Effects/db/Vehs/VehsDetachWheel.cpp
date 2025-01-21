/*
Effect by Reguas
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/EntityFragments.h"

CHAOS_VAR const char *wheels[12] { "wheel_lf",  "wheel_rf",  "wheel_lm1", "wheel_rm1", "wheel_lm2", "wheel_rm2",
	                               "wheel_lm3", "wheel_rm3", "wheel_lr",  "wheel_rr",  "wheel_f",   "wheel_r" };

static void OnStart()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);
		std::vector<int> presentWheels;

		for (const char *wheel : wheels)
		{
			int index = GET_ENTITY_BONE_INDEX_BY_NAME(veh, wheel);
			if (index != -1)
				presentWheels.push_back(index);
		}

		if (!presentWheels.empty())
			Memory::DetachBone(veh, presentWheels[g_Random.GetRandomInt(0, presentWheels.size() - 1)]);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Detach Random Wheel",
		.Id = "vehs_detach_wheel"
	}
);