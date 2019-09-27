#include "stdafx.h"
#include "Main.h"
#include "EffectDispatcher.h"

Main* m_main = nullptr;
EffectDispatcher* m_effectDispatcher = nullptr;

void Main::Init()
{
	if (!m_main)
	{
		m_main = new Main();
	}

	if (!m_effectDispatcher)
	{
		m_effectDispatcher = new EffectDispatcher();
	}

	while (true)
	{
		scriptWait(0);

		if (!DLC2::GET_IS_LOADING_SCREEN_ACTIVE())
		{
			m_effectDispatcher->UpdateTimer();
			m_effectDispatcher->UpdateDisplayedTexts();
			m_effectDispatcher->Draw();
		}
	}
}

void Main::Stop()
{
	if (m_main)
	{
		delete m_main;
		m_main = nullptr;
	}

	if (m_effectDispatcher)
	{
		delete m_effectDispatcher;
		m_effectDispatcher = nullptr;
	}
}

void Main::OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
#ifdef _DEBUG
	static bool CTRLpressed = false;
	if (key == VK_CONTROL)
	{
		CTRLpressed = !isUpNow;
	}
	else if (key == VK_OEM_MINUS && CTRLpressed && !wasDownBefore)
	{
		m_effectDispatcher->DispatchRandomEffect();
	}
#endif
}