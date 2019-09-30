#include "stdafx.h"
#include "Main.h"
#include "EffectDispatcher.h"
#include "Memory.h"

Main* m_main = nullptr;
EffectDispatcher* m_effectDispatcher = nullptr;

bool m_forceDispatchEffectNextFrame = false;

void Main::Init()
{
	if (m_main && m_effectDispatcher)
	{
		return;
	}

	if (!m_main)
	{
		m_main = new Main();
	}

	if (!m_effectDispatcher)
	{
		m_effectDispatcher = new EffectDispatcher();
	}

	MH_EnableHook(MH_ALL_HOOKS);
}

void Main::Loop()
{
	m_effectDispatcher->Reset();

	while (true)
	{
		scriptWait(0);

		if (IS_SCREEN_FADED_OUT())
		{
			continue;
		}

		if (m_forceDispatchEffectNextFrame)
		{
			m_forceDispatchEffectNextFrame = false;

			m_effectDispatcher->ClearEffects();
			m_effectDispatcher->DispatchEffect((EffectType)((int)_EFFECT_ENUM_MAX - 1));
		}

		m_effectDispatcher->UpdateTimer();
		m_effectDispatcher->UpdateEffects();
		m_effectDispatcher->Draw();
	}
}

void Main::Stop()
{
	if (!m_main && !m_effectDispatcher)
	{
		return;
	}

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

	MH_DisableHook(MH_ALL_HOOKS);
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
		m_forceDispatchEffectNextFrame = true;
	}
#endif
}