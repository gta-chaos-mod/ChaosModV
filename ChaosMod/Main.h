#pragma once
#include "DebugMenu.h"
#include "EffectDispatcher.h"
#include "TwitchVoting.h"
#include <Windows.h>
#include <memory>

class Main
{
public:
	bool Init();
	void MainLoop();
	void RunEffectLoop();
	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

private:
	std::shared_ptr<EffectDispatcher> m_effectDispatcher;
	std::unique_ptr<DebugMenu> m_debugMenu;
	std::unique_ptr<TwitchVoting> m_twitchVoting;

	bool m_clearAllEffects = false;
	bool m_pauseTimer = false;
	bool m_clearEffectsShortcutEnabled = false;
	int m_clearEffectsTextTime = 0;
	bool m_disableDrawTimerBar = false;
	bool m_disableDrawEffectTexts = false;
};