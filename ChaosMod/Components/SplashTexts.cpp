#include <stdafx.h>

#include "SplashTexts.h"

SplashTexts::SplashTexts()
{
	m_ullLastTimestamp = GetTickCount64();
}

void SplashTexts::ShowTwitchVotingSplash()
{
	m_iTwitchVotingSplashTime = SPLASH_TEXT_DUR_MS;
}

void SplashTexts::ShowClearEffectsSplash()
{
	m_iClearEffectsSplashTime = SPLASH_TEXT_DUR_MS;
}

void SplashTexts::Run()
{
	DWORD64 ullTimestamp = GetTickCount64();

	if (m_iStartupSplashTime > 0)
	{
		DrawScreenText("Chaos Mod v1.9.1.3\n\nSee credits.txt for list of contributors",
			{ .2f, .3f }, .65f, { 60, 245, 190 }, true);
#ifdef _DEBUG
		DrawScreenText("DEBUG BUILD!", { .2f, .5f }, .7f, { 255, 0, 0 }, true);
#endif

		m_iStartupSplashTime -= ullTimestamp - m_ullLastTimestamp;
	}

	if (m_iTwitchVotingSplashTime > 0)
	{
		DrawScreenText("Twitch Voting Enabled!", { .86f, .7f }, .8f, { 255, 100, 100 }, true);

		m_iTwitchVotingSplashTime -= ullTimestamp - m_ullLastTimestamp;
	}

	if (m_iClearEffectsSplashTime > 0)
	{
		DrawScreenText("Effects Cleared!", { .86f, .86f }, .8f, { 255, 100, 100 }, true);

		m_iClearEffectsSplashTime -= ullTimestamp - m_ullLastTimestamp;
	}

	m_ullLastTimestamp = ullTimestamp;
}