#include <stdafx.h>

#include "SplashTexts.h"

void SplashTexts::Run()
{
	float fFrameTime = GET_FRAME_TIME();

	if (m_fStartupSplashTime > 0)
	{
		DrawScreenText("Chaos Mod v1.10.2\n\nSee credits.txt for a list of contributors",
			{ .2f, .3f }, .65f, { 60, 245, 190 }, true);
#ifdef _DEBUG
		DrawScreenText("DEBUG BUILD!", { .2f, .5f }, .7f, { 255, 0, 0 }, true);
#endif

		m_fStartupSplashTime -= fFrameTime;
	}

	if (m_fTwitchVotingSplashTime > 0)
	{
		DrawScreenText("Twitch Voting Enabled!", { .84f, .7f }, .8f, { 255, 100, 100 }, true);

		m_fTwitchVotingSplashTime -= fFrameTime;
	}

	if (m_fDiscordVotingSplashTime > 0)
	{
		DrawScreenText("Discord Voting Enabled!", { .84f, .6f }, .8f, { 0, 153, 255 }, true);

		m_fDiscordVotingSplashTime -= fFrameTime;
	}

	if (m_fClearEffectsSplashTime > 0)
	{
		DrawScreenText("Effects Cleared!", { .84f, .86f }, .8f, { 255, 100, 100 }, true);

		m_fClearEffectsSplashTime -= fFrameTime;
	}
}

void SplashTexts::ShowInitSplash()
{
	m_fStartupSplashTime = SPLASH_TEXT_DUR_SECS;
}

void SplashTexts::ShowTwitchVotingSplash()
{
	m_fTwitchVotingSplashTime = SPLASH_TEXT_DUR_SECS;
}

void SplashTexts::ShowClearEffectsSplash()
{
	m_fClearEffectsSplashTime = SPLASH_TEXT_DUR_SECS;
}

void SplashTexts::ShowDiscordVotingSplash()
{
	m_fDiscordVotingSplashTime = SPLASH_TEXT_DUR_SECS;
}