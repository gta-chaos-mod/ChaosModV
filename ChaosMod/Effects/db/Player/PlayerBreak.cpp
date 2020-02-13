#include <stdafx.h>

static void OnTick()
{
	DISABLE_ALL_CONTROL_ACTIONS(0);
}

static RegisterEffect registerEffect(EFFECT_BREAK, nullptr, nullptr, OnTick);