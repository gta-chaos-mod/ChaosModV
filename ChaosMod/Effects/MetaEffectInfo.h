#pragma once

#include <string>

#include "EffectsInfo.h"

struct MetaEffectInfo
{
	float EffectDurationModifier = 1;
	float TimerSpeedModifier = 1;
	bool ShouldHideChaosUI = false;
	int AdditionalEffectsToDispatch = 0;
	bool DisableChaos = false;
};

inline MetaEffectInfo g_metaInfo;