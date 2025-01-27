#pragma once

#include "Components/Component.h"
#include "Util/VotingMode.h"

#include <cstdint>

class MetaModifiers : public Component
{
  public:
	float EffectDurationModifier             = 1.f;
	float TimerSpeedModifier                 = 1.f;
	std::uint8_t AdditionalEffectsToDispatch = 0;
	bool HideChaosUI                         = false;
	bool DisableChaos                        = false;
	bool FlipChaosUI                         = false;
	VotingMode VotingModeOverride            = VotingMode::None;
};