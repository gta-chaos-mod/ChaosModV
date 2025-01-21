#pragma once

#include "Effects/Condition/EffectCondition.h"
#include "Effects/EffectCategory.h"
#include "Effects/EffectExecutionType.h"
#include "Effects/EffectGroups.h"

#include <string_view>
#include <unordered_map>
#include <vector>

struct RegisteredEffectMetadata
{
	std::string_view Name;
	std::string_view Id;
	bool IsTimed             = false;
	bool IsShortDuration     = false;
	bool HideRealNameOnStart = false;
#ifdef CHAOSDEBUG
	int DebugShortcutKeycode = 0;
#endif
	std::vector<std::string_view> IncompatibleWith;
	EffectCategory EffectCategory     = EffectCategory::None;
	EffectGroupType EffectGroupType   = EffectGroupType::None;
	EffectExecutionType ExecutionType = EffectExecutionType::Default;
	EffectConditionType ConditionType = EffectConditionType::None;
};

inline std::unordered_map<std::string_view, RegisteredEffectMetadata> g_RegisteredEffectsMetadata;