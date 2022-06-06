#pragma once

#include "Component.h"
#include "Effects/EffectIdentifier.h"

#include <mutex>
#include <queue>
#include <unordered_map>

using DWORD = unsigned long;

class Shortcuts : public Component
{
  private:
	std::unordered_map<int, std::vector<EffectIdentifier>> m_ugAvailableShortcuts;

	std::queue<EffectIdentifier> m_effectQueue;
	std::mutex m_effectQueueMtx;

  protected:
	Shortcuts();

  public:
	virtual void OnRun() override;

	void HandleInput(DWORD ulKey, bool bOnRepeat);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};