#pragma once

#include "Effects/Condition/EffectCondition.h"
#include "Effects/EffectAttributes.h"
#include "Effects/EffectCategory.h"
#include "Effects/EffectGroups.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectTimedType.h"

#include <set>
#include <string>

inline bool g_EnableGroupWeighting = true;

class EffectData
{
  public:
	std::string Name;
	std::string FakeName;
	std::string CustomName;
	EffectIdentifier Id;
	std::string GroupType;
	std::set<std::string> IncompatibleIds;
	float Weight                      = 5.f;
	float WeightMult                  = 5.f;
	float CustomTime                  = -1.f;
	int ShortcutKeycode               = 0;
	EffectTimedType TimedType         = EffectTimedType::NotTimed;
	EffectCategory Category           = EffectCategory::None;
	EffectConditionType ConditionType = EffectConditionType::None;

  private:
	EffectAttributes Attributes {};

  public:
	inline void SetAttribute(EffectAttributes attribute, bool state)
	{
		if (state)
			Attributes |= attribute;
		else
			Attributes &= ~attribute;
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

	inline bool ShouldHideRealNameOnStart() const
	{
		return static_cast<bool>(Attributes & EffectAttributes::HideRealNameOnStart);
	}

	inline bool HasCustomName() const
	{
		return !CustomName.empty();
	}

	float GetEffectWeight() const
	{
		const auto &effectGroup = GroupType;
		auto effectWeight       = Weight;

		return g_EnableGroupWeighting && !effectGroup.empty() && !g_EffectGroups.at(effectGroup).IsPlaceholder
		         ? effectWeight / g_EffectGroups.at(effectGroup).MemberCount * g_EffectGroups.at(effectGroup).WeightMult
		         : effectWeight;
	}
};