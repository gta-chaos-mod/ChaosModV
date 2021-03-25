#include <stdafx.h>

static void OnTick()
{
	HIDE_HUD_AND_RADAR_THIS_FRAME();

	// Ensure player can't use pause menu map either
	DISABLE_CONTROL_ACTION(0, 199, true);
	DISABLE_CONTROL_ACTION(0, 200, true);
}

static RegisterEffect registerEffect(EFFECT_NO_HUD, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No HUD",
		.Id = "no_hud",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_NO_RADAR } 
	}
);