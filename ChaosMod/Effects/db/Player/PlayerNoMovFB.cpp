#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 8, true);
	DISABLE_CONTROL_ACTION(0, 31, true);
	DISABLE_CONTROL_ACTION(0, 32, true);
	DISABLE_CONTROL_ACTION(0, 33, true);
	DISABLE_CONTROL_ACTION(0, 71, true);
	DISABLE_CONTROL_ACTION(0, 72, true);
	DISABLE_CONTROL_ACTION(0, 87, true);
	DISABLE_CONTROL_ACTION(0, 88, true);
	DISABLE_CONTROL_ACTION(0, 126, true);
	DISABLE_CONTROL_ACTION(0, 127, true);
	DISABLE_CONTROL_ACTION(0, 128, true);
	DISABLE_CONTROL_ACTION(0, 129, true);
	DISABLE_CONTROL_ACTION(0, 130, true);
	DISABLE_CONTROL_ACTION(0, 136, true);
	DISABLE_CONTROL_ACTION(0, 137, true);
	DISABLE_CONTROL_ACTION(0, 149, true);
	DISABLE_CONTROL_ACTION(0, 150, true);
	DISABLE_CONTROL_ACTION(0, 151, true);
	DISABLE_CONTROL_ACTION(0, 280, true);
	DISABLE_CONTROL_ACTION(0, 281, true);
}

static RegisterEffect registerEffect(EFFECT_MOV_NO_UD, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Forwards / Backwards Movement",
		.Id = "player_noforwardbackward",
		.IsTimed = true,
		.IsShortDuration = true
	}
);