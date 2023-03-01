#include <stdafx.h>

#include "Info.h"
#include "SplashTexts.h"

#include "Util/Text.h"

void SplashTexts::OnRun()
{
	float fFrameTime = GET_FRAME_TIME();

	static std::string text;
	static bool flag = true;

	
	if (m_fStartupSplashTime > 0)
	{
		if (flag)
		{
			text = std::string("Chaos Mod v" MOD_VERSION " - special build for ")
			     + (g_Random.GetRandomInt(0, 10) == 10 ? "Matto" : "DarkViperAU")
			     + "\n\nSee credits.txt for a list of contributors";
			flag = false;
		}

		DrawScreenText(text, { .25f, .3f }, .65f, { 60, 245, 190 }, true);
#ifdef _DEBUG
		DrawScreenText("DEBUG BUILD!", { .25f, .5f }, .7f, { 255, 0, 0 }, true);
#endif

		m_fStartupSplashTime -= fFrameTime;
	}
	else
	{
		flag = true;
	}

	if (m_fTwitchVotingSplashTime > 0)
	{
		DrawScreenText("Twitch Voting Enabled!", { .86f, .7f }, .8f, { 255, 100, 100 }, true);

		m_fTwitchVotingSplashTime -= fFrameTime;
	}

	if (m_fClearEffectsSplashTime > 0)
	{
		DrawScreenText("Effects Cleared!", { .86f, .86f }, .8f, { 255, 100, 100 }, true);

		m_fClearEffectsSplashTime -= fFrameTime;
	}

	if (m_fTwitchVotingRestartSplashTime > 0)
	{
		DrawScreenText("Voting proxy has crashed and restarted", { .85f, .93f }, .4f, { 225, 67, 240 }, true);

		m_fTwitchVotingRestartSplashTime -= fFrameTime;
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

void SplashTexts::ShowTwitchVotingRestartSplash()
{
	m_fTwitchVotingRestartSplashTime = SPLASH_TEXT_DUR_SECS / 2;
}