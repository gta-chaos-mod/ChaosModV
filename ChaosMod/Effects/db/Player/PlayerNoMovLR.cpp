#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 9, true);
	DISABLE_CONTROL_ACTION(0, 30, true);
	DISABLE_CONTROL_ACTION(0, 34, true);
	DISABLE_CONTROL_ACTION(0, 35, true);
	DISABLE_CONTROL_ACTION(0, 59, true);
	DISABLE_CONTROL_ACTION(0, 63, true);
	DISABLE_CONTROL_ACTION(0, 64, true);
	DISABLE_CONTROL_ACTION(0, 89, true);
	DISABLE_CONTROL_ACTION(0, 90, true);
	DISABLE_CONTROL_ACTION(0, 123, true);
	DISABLE_CONTROL_ACTION(0, 124, true);
	DISABLE_CONTROL_ACTION(0, 125, true);
	DISABLE_CONTROL_ACTION(0, 133, true);
	DISABLE_CONTROL_ACTION(0, 134, true);
	DISABLE_CONTROL_ACTION(0, 146, true);
	DISABLE_CONTROL_ACTION(0, 147, true);
	DISABLE_CONTROL_ACTION(0, 148, true);
	DISABLE_CONTROL_ACTION(0, 152, true);
	DISABLE_CONTROL_ACTION(0, 153, true);
	DISABLE_CONTROL_ACTION(0, 278, true);
	DISABLE_CONTROL_ACTION(0, 279, true);
}

static RegisterEffect registerEffect(EFFECT_MOV_NO_LR, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Left / Right Movement",
		.Id = "player_noleftright",
		.IsTimed = true,
		.IsShortDuration = true
	}
);