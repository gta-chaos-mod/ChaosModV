#pragma once

#include "Components/Component.h"

#include <queue>

class HelpTextQueue : public Component
{
	struct HelpText
	{
		std::string_view Text;
		float TimerSecs;
	};
	std::queue<HelpText> m_HelpTextQueue;

  public:
	virtual void OnModPauseCleanup(PauseCleanupFlags cleanupFlags = {}) override;
	virtual void OnRun() override;

	void DisplayLabel(std::string_view label, std::uint8_t durationSecs);
};