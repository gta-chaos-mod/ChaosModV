#pragma once

#include "EffectGroups.h"
#include "EEffectTimedType.h"
#include "EEffectAttributes.h"

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
	EEffectGroupType GroupType = EEffectGroupType::None;

private:
	EEffectAttributes Attributes {};

public:
	inline void SetAttribute(EEffectAttributes attribute, bool state)
	{
		if (state)
		{
			Attributes |= attribute;
		}
		else
		{
			Attributes &= ~attribute;
		}
	}

	inline bool ExcludedFromVoting() const
	{
		return static_cast<bool>(Attributes & EEffectAttributes::ExcludedFromVoting);
	}

	inline bool HasCustomName() const
	{
		return static_cast<bool>(Attributes & EEffectAttributes::HasCustomName);
	}

	inline bool IsMeta() const
	{
		return static_cast<bool>(Attributes & EEffectAttributes::IsMeta);
	}

	inline bool IsUtility() const
	{
		return static_cast<bool>(Attributes & EEffectAttributes::IsUtility);
	}
};

inline float GetEffectWeight(const EffectData& effectData)
{
	EEffectGroupType effectGroupType = effectData.GroupType;
	float effectWeight = effectData.Weight;

	return g_bEnableGroupWeighting && effectGroupType != EEffectGroupType::None
		? effectWeight / g_dictCurrentEffectGroupMemberCount[effectGroupType] * g_EffectGroups.at(effectGroupType).WeightMult
		: effectWeight;
}