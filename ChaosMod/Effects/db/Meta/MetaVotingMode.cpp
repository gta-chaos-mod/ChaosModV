/*
    Effects by Regynate
*/

#include <stdafx.h>

#include "Components/MetaModifiers.h"
#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	GetComponent<MetaModifiers>()->VotingModeOverride = VotingMode::None;
}

static void OnStart_Majority()
{
	GetComponent<MetaModifiers>()->VotingModeOverride = VotingMode::Majority;
}

// clang-format off
REGISTER_EFFECT(OnStart_Majority, OnStop, nullptr,
	{
		.Name = "Majority Voting",
		.Id = "meta_votingmode_majority",
		.IsTimed = true,
		.IncompatibleWith = { "meta_votingmode_antimajority" },
		.ExecutionType = EffectExecutionType::Meta,
		.ConditionType = EffectConditionType::ProportionalVotingEnabled
	}
);
// clang-format on

static void OnStart_Antimajority()
{
	GetComponent<MetaModifiers>()->VotingModeOverride = VotingMode::Antimajority;
}

// clang-format off
REGISTER_EFFECT(OnStart_Antimajority, OnStop, nullptr,
	{
		.Name = "Minority Voting",
		.Id = "meta_votingmode_antimajority",
		.IsTimed = true,
		.IncompatibleWith = { "meta_votingmode_majority" },
		.ExecutionType = EffectExecutionType::Meta,
		.ConditionType = EffectConditionType::VotingEnabled
	}
);
// clang-format on