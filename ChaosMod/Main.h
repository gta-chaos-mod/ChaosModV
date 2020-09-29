#pragma once

#include "DebugMenu.h"
#include "EffectDispatcher.h"
#include "TwitchVoting.h"
#include "EffectThread.h"

#include <Windows.h>
#include <memory>
#include <map>

inline std::map<EffectType, EffectData> g_enabledEffects;
inline std::vector<EffectThread*> g_effectThreads;

class Main
{
public:
	void Init();
	void MainLoop();
	void RunEffectLoop();
	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);
	void StartEffectThread();

private:
	std::unique_ptr<DebugMenu> m_debugMenu;
	std::unique_ptr<TwitchVoting> m_twitchVoting;

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
};