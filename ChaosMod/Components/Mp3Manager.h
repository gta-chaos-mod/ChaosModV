#pragma once

#include "Components/Component.h"

#include <string>
#include <unordered_map>
#include <vector>

class Mp3Manager : public Component
{
  private:
	std::unordered_map<std::string, std::vector<std::string>> m_EffectSoundFilesCache;

  protected:
	virtual ~Mp3Manager() override;

  private:
	void HandleDirectory(const std::string &dir, const std::string &soundName);

  public:
	void PlayChaosSoundFile(const std::string &soundFileName);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};