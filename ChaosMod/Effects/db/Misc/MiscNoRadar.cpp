#include <stdafx.h>

static void OnStop()
{
	DISPLAY_RADAR(true);
}

static void OnTick()
{
	DISPLAY_RADAR(false);

	// Ensure player can't use pause menu map either
	DISABLE_CONTROL_ACTION(0, 199, true);
	DISABLE_CONTROL_ACTION(0, 200, true);
}

static RegisterEffect registerEffect(EFFECT_NO_RADAR, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Radar",
		.Id = "no_radar",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_NO_HUD }
	}
);