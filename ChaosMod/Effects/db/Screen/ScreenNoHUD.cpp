#include <stdafx.h>

static void OnTick()
{
	HIDE_HUD_AND_RADAR_THIS_FRAME();

	// Ensure player can't use pause menu map either
	DISABLE_CONTROL_ACTION(0, 199, true);
	DISABLE_CONTROL_ACTION(0, 200, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No HUD",
		.Id = "no_hud",
		.IsTimed = true,
		.IncompatibleWith = { "no_radar" } 
	}
);