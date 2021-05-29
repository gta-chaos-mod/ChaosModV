#pragma once

#include "Component.h"

#define SPLASH_TEXT_DUR_MS 15000

class SplashTexts : public Component
{
private:
	DWORD64 m_ullLastTimestamp;

	int m_iStartupSplashTime = SPLASH_TEXT_DUR_MS;
	int m_iTwitchVotingSplashTime = 0;
	int m_iClearEffectsSplashTime = 0;

public:
	void ShowTwitchVotingSplash();
	void ShowClearEffectsSplash();

	SplashTexts();

	virtual void Run() override;
};