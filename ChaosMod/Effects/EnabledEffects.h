#pragma once

#include "Effects/Condition/EffectCondition.h"
#include "Effects/EffectData.h"
#include "Effects/EffectIdentifier.h"

#include <functional>
#include <set>
#include <unordered_map>

using size_t = unsigned long long;

struct EffectData;

class EffectsIdentifierHasher
{
  public:
	size_t operator()(const EffectIdentifier &effectId) const
	{
		return std::hash<std::string>()(effectId.Id());
	}
};

inline std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> g_EnabledEffects;

inline std::vector<EffectData *> GetFilteredEnabledEffects()
{
	std::vector<EffectData *> filteredEffects;

	std::set<EffectConditionType> ensuredConditions;
	for (auto &[conditionType, condition] : g_EffectConditions)
		if (condition->CheckCondition())
			ensuredConditions.insert(conditionType);

	for (auto &[effectId, effectData] : g_EnabledEffects)
		if (effectData.ConditionType == EffectConditionType::None
		    || ensuredConditions.contains(effectData.ConditionType))
			filteredEffects.push_back(&effectData);

	return filteredEffects;
}