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

// Utility: Selects a random effect from a collection using weighted random selection.
// Collection must be iterable with elements having .first (EffectIdentifier) and .second->GetEffectWeight().
// Returns nullptr if the collection is empty or total weight is zero.
template <typename Collection>
inline const EffectIdentifier *SelectWeightedEffect(const Collection &effects, class Random &rng)
{
	float totalWeight = 0.f;
	for (const auto &[effectId, effectData] : effects)
		totalWeight += effectData->GetEffectWeight();

	if (totalWeight <= 0.f)
		return nullptr;

	float chosen                           = rng.GetRandomFloat(0.f, totalWeight);
	totalWeight                            = 0.f;

	const EffectIdentifier *targetEffectId = nullptr;
	for (const auto &[effectId, effectData] : effects)
	{
		totalWeight += effectData->GetEffectWeight();
		if (!targetEffectId && chosen <= totalWeight)
			targetEffectId = &effectId;
	}

	return targetEffectId;
}