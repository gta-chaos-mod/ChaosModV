#include <stdafx.h>

#include "Components/Voting.h"
#include "Effects/Condition/EffectCondition.h"

static bool OnCondition()
{
	return ComponentExists<Voting>() && GetComponent<Voting>()->IsEnabled() && GetComponent<Voting>()->GetVotingMode() == VotingMode::Percentage;
}

REGISTER_EFFECT_CONDITION(EffectConditionType::ProportionalVotingEnabled, OnCondition);