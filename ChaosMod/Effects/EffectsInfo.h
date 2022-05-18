#pragma once

#include "EffectGroups.h"
#include "EEffectExecutionType.h"

#include <unordered_map>

struct EffectInfo
{
	const char* Name;
	const char* Id;
	bool IsTimed = false;
	bool IsShortDuration = false;
	std::vector<EEffectType> IncompatibleWith;
	EEffectGroupType EEffectGroupType = EEffectGroupType::None;
	EEffectExecutionType ExecutionType = EEffectExecutionType::Default;
};

inline std::unordered_map<EEffectType, EffectInfo> g_dictEffectsMap;
