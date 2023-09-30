#pragma once

#include "Effects/EffectCategory.h"
#include "Effects/EffectExecutionType.h"
#include "Effects/EffectGroups.h"

#include <string_view>
#include <unordered_map>
#include <vector>

struct EffectInfo
{
	const char *Name;
	const char *Id;
	bool IsTimed             = false;
	bool IsShortDuration     = false;
	bool HideRealNameOnStart = false;
#ifdef _DEBUG
	int DebugShortcutKeycode = 0;
#endif
	std::vector<std::string_view> IncompatibleWith;
	EffectCategory EffectCategory     = EffectCategory::None;
	EffectGroupType EffectGroupType   = EffectGroupType::None;
	EffectExecutionType ExecutionType = EffectExecutionType::Default;
};

inline std::unordered_map<std::string_view, EffectInfo> g_EffectsMap;