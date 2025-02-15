#pragma once

#include "Components/Component.h"
#include "Effects/EffectSoundPlayOptions.h"

#include <string>

class EffectSoundManager : public Component
{
	std::unordered_map<std::string, std::vector<std::string>> m_EffectSoundFileNamesCache;

  private:
	void HandleDirectory(const std::string &dir, const std::string &soundName);

  protected:
	virtual DWORD64 HandleSound(const std::string &soundFileName) = 0;

  public:
	DWORD64 PlaySoundFile(const std::string &soundFile);
	virtual void SetSoundOptions(DWORD64 soundId, const EffectSoundPlayOptions &soundPlayOptions)
	{
	}
};