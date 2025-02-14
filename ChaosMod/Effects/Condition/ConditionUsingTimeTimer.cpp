#include <stdafx.h>

#include "Components/EffectDispatchTimer.h"
#include "Effects/Condition/EffectCondition.h"

static bool OnCondition()
{
	return ComponentExists<EffectDispatchTimer>() && !GetComponent<EffectDispatchTimer>()->IsUsingDistanceBasedDispatch();
}

REGISTER_EFFECT_CONDITION(EffectConditionType::UsingTimeTimer, OnCondition);