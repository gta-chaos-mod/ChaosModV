#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 21, true);
	DISABLE_CONTROL_ACTION(0, 22, true);
}

static RegisterEffect registerEffect(EFFECT_NO_SPRINT_JUMP, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "No Sprint & No Jump",
		.Id = "player_nosprint",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_SUPER_RUN }
	}
);