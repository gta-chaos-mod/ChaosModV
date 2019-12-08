#pragma once
#include <Windows.h>
#include <memory>

class DebugMenu;
class EffectDispatcher;
class Memory;
class TwitchVoting;

class Main
{
public:
	Main(std::shared_ptr<Memory> memory);
	~Main();

	bool Init();
	void MainLoop();
	void RunEffectLoop();
	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

private:
	const std::shared_ptr<Memory> m_memory;
	std::shared_ptr<EffectDispatcher> m_effectDispatcher;
	std::unique_ptr<DebugMenu> m_debugMenu;
	std::unique_ptr<TwitchVoting> m_twitchVotingMgr;

	bool m_clearAllEffects = false;
	bool m_pauseTimer = false;
	bool m_clearEffectsShortcutEnabled = false;
	int m_clearEffectsTextTime = 0;
	bool m_disableDrawTimerBar = false;
	bool m_disableDrawEffectTexts = false;
};