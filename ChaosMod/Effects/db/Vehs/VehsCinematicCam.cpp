#include <stdafx.h>

static void OnStop()
{
	SET_CINEMATIC_MODE_ACTIVE(false);
	SET_PLAYER_CAN_DO_DRIVE_BY(PLAYER_ID(), true);
}

static void OnTick()
{
	SET_PLAYER_CAN_DO_DRIVE_BY(PLAYER_ID(), false);

	SET_CINEMATIC_MODE_ACTIVE(true);

	DISABLE_CONTROL_ACTION(0, 80, true);

	if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		DISABLE_CONTROL_ACTION(0, 27, true);
		DISABLE_CONTROL_ACTION(0, 0, true);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Cinematic Vehicle Cam",
		.Id = "player_forcedcinematiccam",
		.IsTimed = true,
		.IncompatibleWith = { "player_firstperson" }
	}
);