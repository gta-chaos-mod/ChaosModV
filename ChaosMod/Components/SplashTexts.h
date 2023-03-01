#pragma once

#include <list>

#include "Component.h"
#include "Util/Text.h"

#define SPLASH_TEXT_DUR_SECS 10

class SplashTexts : public Component
{
  private:
	struct SplashText
	{
		const std::string m_szText;
		const ScreenTextVector m_textPos;
		const float m_fScale;
		const ScreenTextColor m_textColor;
		float m_fTime;

		SplashText(const std::string &szText, const ScreenTextVector &textPos, float fScale,
		           const ScreenTextColor &textColor, float fTime)
		    : m_szText(szText), m_textPos(textPos), m_fScale(fScale), m_textColor(textColor), m_fTime(fTime)
		{
		}
	};

	std::list<SplashText> m_activeSplashes;

  public:
	virtual void OnRun() override;
	virtual void OnModPauseCleanup() override;

	void ShowSplash(const std::string &szText, const ScreenTextVector &textPos, float fScale, ScreenTextColor textColor,
	                float fTime);
	void ShowInitSplash();
	void ShowTwitchVotingSplash();
	void ShowClearEffectsSplash();
	void ShowTwitchVotingRestartSplash();

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};