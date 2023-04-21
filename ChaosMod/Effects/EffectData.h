#pragma once

#include "EffectAttributes.h"
#include "EffectCategory.h"
#include "EffectGroups.h"
#include "EffectTimedType.h"

#include <string>
#include <vector>

inline bool g_EnableGroupWeighting = true;

struct EffectData
{
	std::vector<std::string> IncompatibleIds;
	std::string Name;
	std::string FakeName;
	std::string CustomName;
	std::string Id;
	float Weight                  = 5.f;
	int CustomTime                = -1;
	int WeightMult                = 5;
	int ShortcutKeycode           = 0;
	EffectTimedType TimedType     = EffectTimedType::Unk;
	EffectCategory EffectCategory = EffectCategory::None;
	std::string GroupType;

  private:
	EffectAttributes Attributes {};

  public:
	inline void SetAttribute(EffectAttributes attribute, bool state)
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

	inline bool IsExcludedFromVoting() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::ExcludedFromVoting) || IsMeta() || IsUtility()
		    || IsTemporary();
	}

	inline bool IsHidden() const
	{
		return IsTemporary() || TimedType == EffectTimedType::Permanent;
	}

	inline bool HasCustomName() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::HasCustomName);
	}

	inline bool IsMeta() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::IsMeta);
	}

	inline bool IsUtility() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::IsUtility);
	}

	inline bool IsTemporary() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::IsTemporary);
	}
};

inline float GetEffectWeight(const EffectData &effectData)
{
	const auto &effectGroup = effectData.GroupType;
	auto effectWeight       = effectData.Weight;

	return g_EnableGroupWeighting && !effectGroup.empty() && !g_EffectGroups.at(effectGroup).IsPlaceholder
	         ? effectWeight / g_EffectGroups.at(effectGroup).MemberCount * g_EffectGroups.at(effectGroup).WeightMult
	         : effectWeight;
}