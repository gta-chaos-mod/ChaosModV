#include <stdafx.h>

#include "Effects/Condition/EffectCondition.h"

static bool OnCondition()
{
	return !IsEnhanced();
}

REGISTER_EFFECT_CONDITION(EffectConditionType::EnhancedShader, OnCondition, "Shader effects do not work on Enhanced");