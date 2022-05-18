#pragma once

#include "Effects/EffectGroups.h"
#include "Effects/EEffectExecutionType.h"
#include "Effects/EEffectCategory.h"

#include <unordered_map>
#include <vector>
#include <string_view>

struct EffectInfo
{
	const char* Name;
	const char* Id;
	bool IsTimed = false;
	bool IsShortDuration = false;
	std::vector<std::string_view> IncompatibleWith;
	EEffectCategory EffectCategory = EEffectCategory::None;
	EEffectGroupType EffectGroupType = EEffectGroupType::None;
	EEffectExecutionType ExecutionType = EEffectExecutionType::Default;
};

inline std::unordered_map<std::string_view, EffectInfo> g_dictEffectsMap;