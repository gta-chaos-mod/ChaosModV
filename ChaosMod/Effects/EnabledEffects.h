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

inline bool IsEffectFilteredOut(EffectIdentifier id)
{
	std::set<EffectConditionType> ensuredConditions;
	for (auto &[conditionType, condition] : g_EffectConditions)
		if (condition->CheckCondition())
			ensuredConditions.insert(conditionType);

	if (!g_EnabledEffects.contains(id))
		return false;

	const auto &effectData = g_EnabledEffects.at(id);

	return effectData.ConditionType != EffectConditionType::None
	    && !ensuredConditions.contains(effectData.ConditionType);
}

inline std::string GetFilterReason(EffectIdentifier id)
{
	if (!g_EnabledEffects.contains(id))
		return "";

	const auto &effectData = g_EnabledEffects.at(id);

	if (effectData.ConditionType == EffectConditionType::None)
		return "";

	return g_EffectConditions.at(effectData.ConditionType)->GetFailReason();
}