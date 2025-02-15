#pragma once

#include "Components/EffectSound/EffectSoundManager.h"

#include <string>

class EffectSoundMCI : public EffectSoundManager
{
  public:
	virtual DWORD64 HandleSound(const std::string &soundFile) override;
};