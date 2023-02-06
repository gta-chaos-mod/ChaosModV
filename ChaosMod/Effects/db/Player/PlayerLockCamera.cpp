/*
    Effect by dzwdz, modified
*/

#include <stdafx.h>

static void OnTick()
{
	_SET_GAMEPLAY_CAM_RELATIVE_ROTATION(0, 0, 0);

	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		DISABLE_CONTROL_ACTION(0, 2, true); // Look UD
	}

	if (!IS_PLAYER_FREE_AIMING(PLAYER_ID()))
	{
		DISABLE_CONTROL_ACTION(0, 1, true); // Look LR
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Lock Camera",
		.Id = "player_lockcamera",
		.IsTimed = true
	}
);
