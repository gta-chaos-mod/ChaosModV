#pragma once

#include <string>

#include "EffectsInfo.h"

struct MetaEffectInfo
{
	float effectDurationModifier = 1;
	float timerSpeedModifier = 1;
	bool shouldHideChaosUI = false;
	bool additionalEffectsToDispatch = 0;
};
