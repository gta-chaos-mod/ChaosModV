/*
	Effect by yzimroni
*/

#include <stdafx.h>

static void OnTick()
{
	// Disable all brake and descend actions
	DISABLE_CONTROL_ACTION(0, 72, true);
	DISABLE_CONTROL_ACTION(0, 76, true);
	DISABLE_CONTROL_ACTION(0, 88, true);
	DISABLE_CONTROL_ACTION(0, 138, true);
	DISABLE_CONTROL_ACTION(0, 139, true);
	DISABLE_CONTROL_ACTION(0, 152, true);
	DISABLE_CONTROL_ACTION(0, 153, true);
}

static RegisterEffect registerEffect(EFFECT_VEH_NO_BRAKES, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Braking Allowed",
		.Id = "playerveh_nobrakes",
		.IsTimed = true
	}
);