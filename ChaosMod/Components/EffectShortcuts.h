#pragma once

#include "Components/Component.h"

#include "Effects/EffectIdentifier.h"

#include <mutex>
#include <queue>
#include <unordered_map>

using DWORD = unsigned long;

class EffectShortcuts : public Component
{
  private:
	std::unordered_map<int, std::vector<EffectIdentifier>> m_AvailableShortcuts;

	std::queue<EffectIdentifier> m_EffectQueue;
	std::mutex m_EffectQueueMtx;

  protected:
	EffectShortcuts();

  public:
	virtual void OnRun() override;

	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed) override;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};