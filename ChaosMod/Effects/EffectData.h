#pragma once

#include "EffectGroups.h"
#include "EEffectTimedType.h"

#include <string>
#include <vector>

inline bool g_bEnableGroupWeighting = true;

struct EffectData
{
	EEffectTimedType TimedType = EEffectTimedType::Unk;
	int CustomTime = -1;
	int WeightMult = 5;
	float Weight = WeightMult;
	bool ExcludedFromVoting = false;
	std::string Name;
	std::string FakeName;
	bool HasCustomName = false;
	std::string CustomName;
	std::string Id;
	std::vector<std::string> IncompatibleIds;
	bool IsMeta = false;
	EEffectGroupType EEffectGroupType = EEffectGroupType::None;
};

inline float GetEffectWeight(const EffectData& effectData)
{
	EEffectGroupType effectGroupType = effectData.EEffectGroupType;
	float effectWeight = effectData.Weight;

	return g_bEnableGroupWeighting && effectGroupType != EEffectGroupType::None
		? effectWeight / g_dictCurrentEffectGroupMemberCount[effectGroupType] * g_EffectGroups.at(effectGroupType).WeightMult
		: effectWeight;
}