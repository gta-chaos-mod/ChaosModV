#pragma once

#include "EffectGroups.h"
#include "EffectTimedType.h"

#include <string>
#include <vector>

struct EffectData
{
	EffectTimedType TimedType = EffectTimedType::TIMED_UNK;
	int CustomTime = -1;
	int WeightMult = 5;
	float Weight = WeightMult;
	bool ExcludedFromVoting = false;
	std::string Name;
	bool HasCustomName = false;
	std::string CustomName;
	std::string Id;
	std::vector<std::string> IncompatibleIds;
	bool IsMeta = false;
	EffectGroupType EffectGroupType = EffectGroupType::NONE;
};

inline float GetEffectWeight(const EffectData& effectData)
{
	EffectGroupType effectGroupType = effectData.EffectGroupType;
	float effectWeight = effectData.Weight;

	return effectGroupType != EffectGroupType::NONE
		? effectWeight / g_currentEffectGroupMemberCount[effectGroupType] * g_effectGroups.at(effectGroupType).WeightMult
		: effectWeight;
}