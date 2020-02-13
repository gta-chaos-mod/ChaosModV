#include <stdafx.h>

static void OnTick()
{
	SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f, .0f);
}

static RegisterEffect registerEffect(EFFECT_NO_PEDS, nullptr, nullptr, OnTick);