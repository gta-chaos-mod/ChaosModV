#pragma once

#include "Component.h"

#define SPLASH_TEXT_DUR_SECS 10

class SplashTexts : public Component
{
private:
	float m_fStartupSplashTime = 0.f;
	float m_fTwitchVotingSplashTime = 0.f;
	float m_fClearEffectsSplashTime = 0.f;

public:
	virtual void Run() override;

	void ShowInitSplash();
	void ShowTwitchVotingSplash();
	void ShowClearEffectsSplash();
};