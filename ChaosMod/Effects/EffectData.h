#pragma once

#include "EffectGroups.h"
#include "EEffectTimedType.h"

#include <string>
#include <vector>

inline bool g_bEnableGroupWeighting = true;

struct EffectData
{
	std::vector<std::string> IncompatibleIds;
	std::string Name;
	std::string FakeName;
	std::string CustomName;
	std::string Id;
	float Weight = WeightMult;
	int CustomTime = -1;
	int WeightMult = 5;
	int Shortcut = 0;
	EEffectTimedType TimedType = EEffectTimedType::Unk;
	EEffectGroupType EEffectGroupType = EEffectGroupType::None;
	bool ExcludedFromVoting = false;
	bool HasCustomName = false;
	bool IsMeta = false;
	bool IsUtility = false;
};

inline float GetEffectWeight(const EffectData& effectData)
{
	EEffectGroupType effectGroupType = effectData.EEffectGroupType;
	float effectWeight = effectData.Weight;

	return g_bEnableGroupWeighting && effectGroupType != EEffectGroupType::None
		? effectWeight / g_dictCurrentEffectGroupMemberCount[effectGroupType] * g_EffectGroups.at(effectGroupType).WeightMult
		: effectWeight;
}