#pragma once

#include "DebugMenu.h"
#include "TwitchVoting.h"
#include "ProxyManager.h"

#include <Windows.h>
#include <memory>
#include <map>

class Main
{
public:
	void Init();
	void Loop();
	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

private:
	std::unique_ptr<DebugMenu> m_debugMenu;
	std::unique_ptr<TwitchVoting> m_twitchVoting;
	std::unique_ptr<ProxyManager> m_proxyManager;

	bool m_clearAllEffects = false;
	bool m_pauseTimer = false;
	bool m_clearEffectsShortcutEnabled = false;
	int m_clearEffectsTextTime = 0;
	bool m_disableDrawTimerBar = false;
	bool m_disableDrawEffectTexts = false;
	bool m_toggleModShortcutEnabled = false;
	bool m_disableMod = false;
	bool m_enableDebugMenu = false;
	bool m_enablePauseTimerShortcut = false;

	void Reset();
};