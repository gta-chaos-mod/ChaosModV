/*
    Effect By Rylxnd
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	_DELETE_WAYPOINT();
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Remove Waypoint",
		.Id = "misc_nowaypoint"
	}
);