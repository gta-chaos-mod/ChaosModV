#pragma once

#include "Components/Component.h"

#include "Util/ScriptText.h"

#include <cstdint>
#include <list>

class SplashTexts : public Component
{
	struct SplashText
	{
		const std::string Text;
		const ScreenTextVector TextPos;
		const float Scale;
		const Color TextColor;
		float Time;

		SplashText(const std::string &text, const ScreenTextVector &textPos, float scale,
		           const Color &textColor, float time)
		    : Text(text), TextPos(textPos), Scale(scale), TextColor(textColor), Time(time)
		{
		}
	};

	std::list<SplashText> m_ActiveSplashes;

	bool m_EnableSplashTexts = true;

  public:
	SplashTexts();

	virtual void OnRun() override;
	virtual void OnModPauseCleanup(PauseCleanupFlags cleanupFlags = {}) override;

	void ShowSplash(const std::string &text, const ScreenTextVector &textPos, float scale, Color textColor,
	                std::uint8_t timeSecs = 10);
	void ShowVotingSplash();
	void ShowClearEffectsSplash();
};