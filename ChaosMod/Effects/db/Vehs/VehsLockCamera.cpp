/*
	Effect by dzwdz
*/

#include <stdafx.h>

static void OnTick()
{
	if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
		_SET_GAMEPLAY_CAM_RELATIVE_ROTATION((float)0.0, (float)0.0, (float)0.0);
}

static RegisterEffect registerEffect(EFFECT_VEH_LOCKCAMERA, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Lock Vehicle Camera",
		.Id = "veh_lockcamera",
		.IsTimed = true
	}
);