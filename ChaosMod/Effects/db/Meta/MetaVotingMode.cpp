/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

static void OnStop()
{
	MetaModifiers::m_sCurrentVotingMode = "percentage";
}

static void OnStart_Majority()
{
	MetaModifiers::m_sCurrentVotingMode = "majority";
}

// clang-format off
REGISTER_EFFECT(OnStart_Majority, OnStop, nullptr, EffectInfo
	{
		.Name = "Not Rigged",
		.Id = "meta_votingmode_majority",
		.IsTimed = true,
		.IncompatibleWith = { "meta_votingmode_antimajority" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);
// clang-format on

static void OnStart_Antimajority()
{
	MetaModifiers::m_sCurrentVotingMode = "antimajority";
}

// clang-format off
REGISTER_EFFECT(OnStart_Antimajority, OnStop, nullptr, EffectInfo
	{
		.Name = "Rigged",
		.Id = "meta_votingmode_antimajority",
		.IsTimed = true,
		.IncompatibleWith = { "meta_votingmode_majority" },
		.ExecutionType = EEffectExecutionType::Meta
	}
);
// clang-format on