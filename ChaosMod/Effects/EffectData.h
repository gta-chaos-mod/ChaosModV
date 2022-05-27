#pragma once

#include "EEffectAttributes.h"
#include "EEffectCategory.h"
#include "EEffectTimedType.h"
#include "EffectGroups.h"

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
	float Weight                   = 5.f;
	int CustomTime                 = -1;
	int WeightMult                 = 5;
	int ShortcutKeycode            = 0;
	EEffectTimedType TimedType     = EEffectTimedType::Unk;
	EEffectCategory EffectCategory = EEffectCategory::None;
	std::string GroupType;

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
		return static_cast<bool>(Attributes & EEffectAttributes::ExcludedFromVoting) || IsMeta() || IsUtility();
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

inline float GetEffectWeight(const EffectData &effectData)
{
	const auto &effectGroup = effectData.GroupType;
	auto effectWeight       = effectData.Weight;

	return g_bEnableGroupWeighting && !effectGroup.empty() && !g_dictEffectGroups.at(effectGroup).IsPlaceholder
	         ? effectWeight / g_dictEffectGroupMemberCount.at(effectGroup)
	               * g_dictEffectGroups.at(effectGroup).WeightMult
	         : effectWeight;
}