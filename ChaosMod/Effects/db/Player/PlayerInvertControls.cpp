/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static Vehicle lastVeh;

static void OnTick()
{
	Vehicle veh = GET_VEHICLE_PED_IS_USING(PLAYER_PED_ID());
	if (veh != lastVeh)
	{
		_SET_VEHICLE_CONTROLS_INVERTED(lastVeh, false);
		_SET_VEHICLE_CONTROLS_INVERTED(veh, true);
		lastVeh = veh;
	}
}

static void OnStop()
{
	_SET_VEHICLE_CONTROLS_INVERTED(lastVeh, false);
	lastVeh = 0;
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Invert Vehicle Controls",
		.Id = "player_invert_veh_controls",
		.IsTimed = true,
		.IsShortDuration = true
	}
);
