#include <stdafx.h>

static void OnTick()
{
	static unsigned int constexpr DRIVING_STYLE = 1024; // "Drive in reverse gear" bit

	for (auto ped : GetAllPeds())
	{
		if(ped == PLAYER::PLAYER_PED_ID() || !PED::IS_PED_IN_ANY_VEHICLE(ped, false))
			continue;

		TASK::SET_DRIVE_TASK_DRIVING_STYLE(ped, 1024);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_DRIVE_BACKWARDS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Peds Drive Backwards",
		.Id = "peds_drive_backwards",
		.IsTimed = true
	}
);