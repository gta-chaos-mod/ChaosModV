#include <stdafx.h>

static void OnTick()
{
	DISABLE_CONTROL_ACTION(0, 21, true);
}

static RegisterEffect registerEffect(EFFECT_NO_SPRINT, nullptr, nullptr, OnTick);