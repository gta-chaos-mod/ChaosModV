#include <stdafx.h>

static void OnStop()
{
	SET_FOLLOW_PED_CAM_VIEW_MODE(1);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(1);
}

static void OnTick()
{
	SET_CINEMATIC_MODE_ACTIVE(false);

	SET_FOLLOW_PED_CAM_VIEW_MODE(4);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(4);
	SET_FOLLOW_VEHICLE_CAM_ZOOM_LEVEL(4);

	DISABLE_CONTROL_ACTION(0, 0, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "First Person",
		.Id = "player_firstperson",
		.IsTimed = true,
		.IncompatibleWith = { "player_forcedcinematiccam" }
	}
);