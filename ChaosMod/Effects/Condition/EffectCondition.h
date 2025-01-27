#pragma once

#include "Util/Logging.h"
#include "Util/MacroConcat.h"

enum class EffectConditionType
{
	None,
	VotingEnabled, // Voting is enabled
	ProportionalVotingEnabled
};

#define REGISTER_EFFECT_CONDITION(conditionType, condition)                           \
	namespace                                                                         \
	{                                                                                 \
		EffectCondition CONCAT(_effectCondition, __LINE__)(conditionType, condition); \
	}

class EffectCondition;

inline std::unordered_map<EffectConditionType, EffectCondition *> g_EffectConditions;

class EffectCondition
{
	const EffectConditionType ConditionType = EffectConditionType::None;
	const std::function<bool()> Condition;

  public:
	EffectCondition(EffectConditionType conditionType, std::function<bool()> condition)
	    : ConditionType(conditionType), Condition(condition)
	{
		if (conditionType == EffectConditionType::None)
		{
			LOG("ERROR: conditionType == None");
			return;
		}

		if (g_EffectConditions.contains(conditionType))
		{
			LOG("ERROR: Duplicate conditionType " << static_cast<int>(ConditionType));
			return;
		}

		if (!condition)
		{
			LOG("ERROR: ConditionType " << static_cast<int>(ConditionType) << " has empty condition");
			return;
		}

		g_EffectConditions[ConditionType] = this;
	}

	~EffectCondition()
	{
		g_EffectConditions.erase(ConditionType);
	}

	EffectCondition(const EffectCondition &)            = delete;
	EffectCondition &operator=(const EffectCondition &) = delete;

	bool CheckCondition()
	{
		return Condition();
	}
};