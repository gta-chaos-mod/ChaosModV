/*
	Effect by dzwdz
*/

#include <stdafx.h>

static void OnTick()
{
	if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		_SET_GAMEPLAY_CAM_RELATIVE_ROTATION(0, 0, 0);
	}
	else
	{
		SET_GAMEPLAY_CAM_RELATIVE_HEADING(0);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_LOCKCAMERA, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Lock Camera",
		.Id = "player_lockcamera",
		.IsTimed = true
	}
);