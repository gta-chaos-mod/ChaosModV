#include <stdafx.h>

#include "Components/Voting.h"
#include "Effects/Condition/EffectCondition.h"

static bool OnCondition()
{
	return ComponentExists<Voting>() && GetComponent<Voting>()->IsEnabled();
}

REGISTER_EFFECT_CONDITION(EffectConditionType::VotingEnabled, OnCondition, "Voting is not enabled");