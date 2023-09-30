#include <stdafx.h>

#include "SplashTexts.h"

#include "Info.h"

#include "Util/OptionsManager.h"

static bool ms_InitalSplashShown = false;

SplashTexts::SplashTexts()
{
	m_EnableSplashTexts =
	    g_OptionsManager.GetConfigValue({ "EnableModSplashTexts" }, OPTION_DEFAULT_ENABLE_SPLASH_TEXTS);

	if (ms_InitalSplashShown)
	{
		return;
	}

	ShowSplash("Chaos Mod v" MOD_VERSION "\n\nSee credits.txt for a list of contributors", { .2f, .3f }, .65f,
	           { 60, 245, 190 });
#ifdef _DEBUG
	ShowSplash("DEBUG BUILD!", { .2f, .5f }, .7f, { 255, 0, 0 });
#endif

	ms_InitalSplashShown = true;
}

void SplashTexts::OnModPauseCleanup()
{
	m_ActiveSplashes.clear();
}

void SplashTexts::OnRun()
{
	float frameTime = GET_FRAME_TIME();

	for (std::list<SplashText>::iterator it = m_ActiveSplashes.begin(); it != m_ActiveSplashes.end();)
	{
		DrawScreenText(it->Text, it->TextPos, it->Scale, it->TextColor, true);
		it->Time -= frameTime;

		if (it->Time <= 0)
		{
			it = m_ActiveSplashes.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void SplashTexts::ShowSplash(const std::string &text, const ScreenTextVector &textPos, float scale,
                             ScreenTextColor textColor, std::uint8_t time)
{
	if (!m_EnableSplashTexts)
	{
		return;
	}

	m_ActiveSplashes.emplace_back(text, textPos, scale, textColor, time);
}

void SplashTexts::ShowVotingSplash()
{
	ShowSplash("Voting Enabled!", { .86f, .7f }, .8f, { 255, 100, 100 });
}

void SplashTexts::ShowClearEffectsSplash()
{
	ShowSplash("Effects Cleared!", { .86f, .86f }, .8f, { 255, 100, 100 });
}