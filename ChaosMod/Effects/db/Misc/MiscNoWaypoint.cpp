/*
    Effect By Rylxnd
*/

#include <stdafx.h>

static void OnStart()
{
	_DELETE_WAYPOINT();
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Remove Waypoint",
		.Id = "misc_nowaypoint"
	}
);