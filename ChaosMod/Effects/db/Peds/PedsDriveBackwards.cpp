#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	static unsigned int constexpr DRIVING_STYLE = 1024; // "Drive in reverse gear" bit

	for (auto ped : GetAllPeds())
	{
		if (ped == PLAYER::PLAYER_PED_ID() || !PED::IS_PED_IN_ANY_VEHICLE(ped, false))
			continue;

		TASK::SET_DRIVE_TASK_DRIVING_STYLE(ped, 1024);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Peds Drive Backwards",
		.Id = "peds_drive_backwards",
		.IsTimed = true
	}
);