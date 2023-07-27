#pragma once

#include "Components/Component.h"

#include "Util/ScriptText.h"

#include <list>

class SplashTexts : public Component
{
  private:
	struct SplashText
	{
		const std::string Text;
		const ScreenTextVector TextPos;
		const float Scale;
		const ScreenTextColor TextColor;
		float Time;

		SplashText(const std::string &text, const ScreenTextVector &textPos, float scale,
		           const ScreenTextColor &textColor, float time)
		    : Text(text), TextPos(textPos), Scale(scale), TextColor(textColor), Time(time)
		{
		}
	};

	std::list<SplashText> m_ActiveSplashes;

  public:
	virtual void OnRun() override;
	virtual void OnModPauseCleanup() override;

	void ShowSplash(const std::string &text, const ScreenTextVector &textPos, float scale, ScreenTextColor textColor,
	                float time);
	void ShowInitSplash();
	void ShowVotingSplash();
	void ShowClearEffectsSplash();

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};