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
	void DisplayLabel(std::string_view label, std::uint8_t durationSecs);

	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;
};