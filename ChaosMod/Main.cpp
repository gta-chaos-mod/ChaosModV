#include <stdafx.h>

#include "Main.h"
#include "Mp3Manager.h"

#include "Effects/EffectConfig.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"
#include "Memory/Hooks/ShaderHook.h"
#include "Memory/Misc.h"
#include "Memory/Shader.h"

#include "Components/DebugMenu.h"
#include "Components/EffectDispatcher.h"
#include "Components/Failsafe.h"
#include "Components/Shortcuts.h"
#include "Components/SplashTexts.h"
#include "Components/TwitchVoting.h"

#include "Util/OptionsManager.h"
#include "Util/PoolSpawner.h"

static bool ms_bClearAllEffects             = false;
static bool ms_bClearEffectsShortcutEnabled = false;
static bool ms_bToggleModShortcutEnabled    = false;
static bool ms_bDisableMod                  = false;
static bool ms_bEnablePauseTimerShortcut    = false;
static bool ms_bHaveLateHooksRan            = false;

_NODISCARD static std::array<BYTE, 3> ParseConfigColorString(const std::string &szColorText)
{
	// Format: #ARGB
	std::array<BYTE, 3> rgColors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		Util::TryParse<BYTE>(szColorText.substr(i, 2), rgColors[j++], 16);
	}

	return rgColors;
}

static void ParseEffectsFile()
{
	g_dictEnabledEffects.clear();

	EffectConfig::ReadConfig("chaosmod/effects.ini", g_dictEnabledEffects);
}

static void Reset()
{
	// Check if this isn't the first time this is being run
	if (ComponentExists<EffectDispatcher>())
	{
		LOG("Mod has been disabled using shortcut!");
	}

	ClearEntityPool();

	Mp3Manager::ResetCache();

	LuaScripts::Unload();

	for (auto pComponent : g_rgComponents)
	{
		pComponent->OnModPauseCleanup();
	}
}

static void Init()
{
	static std::streambuf *c_pOldStreamBuf;
	if (DoesFileExist("chaosmod\\.enableconsole"))
	{
		if (GetConsoleWindow())
		{
			system("cls");
		}
		else
		{
			LOG("Creating log console");

			AllocConsole();

			SetConsoleTitle(L"ChaosModV");
			DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

			c_pOldStreamBuf = std::cout.rdbuf();

			g_ConsoleOut    = std::ofstream("CONOUT$");
			std::cout.rdbuf(g_ConsoleOut.rdbuf());

			std::cout.clear();

			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			DWORD ulConMode;
			GetConsoleMode(handle, &ulConMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ulConMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
	}
	else if (GetConsoleWindow())
	{
		LOG("Destroying log console");

		std::cout.rdbuf(c_pOldStreamBuf);

		g_ConsoleOut.close();

		FreeConsole();
	}

	LOG("Parsing config files");
	ParseEffectsFile();

	g_OptionsManager.Reset();

	ms_bClearEffectsShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnableClearEffectsShortcut", OPTION_DEFAULT_SHORTCUT_CLEAR_EFFECTS);
	ms_bToggleModShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnableToggleModShortcut", OPTION_DEFAULT_SHORTCUT_TOGGLE_MOD);
	ms_bEnablePauseTimerShortcut =
	    g_OptionsManager.GetConfigValue<bool>("EnablePauseTimerShortcut", OPTION_DEFAULT_SHORTCUT_PAUSE_TIMER);

	g_bEnableGroupWeighting =
	    g_OptionsManager.GetConfigValue<bool>("EnableGroupWeightingAdjustments", OPTION_DEFAULT_GROUP_WEIGHTING);

	const auto &rgTimerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTimerColor", OPTION_DEFAULT_BAR_COLOR));
	const auto &rgTextColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTextColor", OPTION_DEFAULT_TEXT_COLOR));
	const auto &rgEffectTimerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTimedTimerColor", OPTION_DEFAULT_TIMED_COLOR));

	LOG("Running custom scripts");
	LuaScripts::Load();

	g_Random.SetSeed(g_OptionsManager.GetConfigValue<int>("Seed", 0));

	LOG("Initializing effects dispatcher");
	InitComponent<EffectDispatcher>(rgTimerColor, rgTextColor, rgEffectTimerColor);

	InitComponent<DebugMenu>();

	LOG("Initializing shortcuts");
	InitComponent<Shortcuts>();

	LOG("Initializing Twitch voting");
	InitComponent<TwitchVoting>(rgTextColor);

	LOG("Initializing Failsafe");
	InitComponent<Failsafe>();

	LOG("Completed init!");

	if (ComponentExists<TwitchVoting>() && GetComponent<TwitchVoting>()->IsEnabled() && ComponentExists<SplashTexts>())
	{
		GetComponent<SplashTexts>()->ShowTwitchVotingSplash();
	}
}

static void MainRun()
{
	if (!ms_bHaveLateHooksRan)
	{
		ms_bHaveLateHooksRan = true;

		Memory::RunLateHooks();
	}

	g_MainThread = GetCurrentFiber();

	EffectThreads::ClearThreads();

	Reset();

	InitComponent<SplashTexts>();
	GetComponent<SplashTexts>()->ShowInitSplash();

	ms_bDisableMod = g_OptionsManager.GetConfigValue<bool>("DisableStartup", OPTION_DEFAULT_DISABLE_STARTUP);

	Init();

	bool c_bJustReenabled = false;

	while (true)
	{
		WAIT(0);

		if (!EffectThreads::IsAnyThreadRunningOnStart())
		{
			if (ms_bDisableMod && !c_bJustReenabled)
			{
				c_bJustReenabled = true;
				Reset();

				continue;
			}
			else if (c_bJustReenabled)
			{
				if (!ms_bDisableMod)
				{
					c_bJustReenabled = false;

					// Clear log
					g_Log            = std::ofstream("chaosmod/chaoslog.txt");

					// Restart the main part of the mod completely
					Init();
				}

				continue;
			}

			if (ms_bClearAllEffects)
			{
				ms_bClearAllEffects = false;
				GetComponent<EffectDispatcher>()->Reset();
				ClearEntityPool();
			}
		}
		else if (IS_SCREEN_FADED_OUT())
		{
			// Prevent potential softlock for certain effects
			SET_TIME_SCALE(1.f);
			Hooks::DisableScriptThreadBlock();
			WAIT(100);

			continue;
		}

		for (auto pComponent : g_rgComponents)
		{
			pComponent->OnRun();
		}
	}
}

namespace Main
{
	void OnRun()
	{
		SetUnhandledExceptionFilter(CrashHandler);

		MainRun();
	}

	void OnCleanup()
	{
		LuaScripts::Unload();
	}

	void OnKeyboardInput(DWORD ulKey, WORD usRepeats, BYTE ucScanCode, BOOL bIsExtended, BOOL bIsWithAlt,
	                     BOOL bWasDownBefore, BOOL bIsUpNow)
	{
		static bool c_bIsCtrlPressed = false;

		if (ulKey == VK_CONTROL)
		{
			c_bIsCtrlPressed = !bIsUpNow;
		}
		else if (c_bIsCtrlPressed && !bWasDownBefore)
		{
			if (ulKey == VK_OEM_MINUS && ms_bClearEffectsShortcutEnabled)
			{
				ms_bClearAllEffects = true;

				if (ComponentExists<SplashTexts>())
				{
					GetComponent<SplashTexts>()->ShowClearEffectsSplash();
				}
			}
			else if (ulKey == VK_OEM_PERIOD && ms_bEnablePauseTimerShortcut && ComponentExists<EffectDispatcher>())
			{
				GetComponent<EffectDispatcher>()->m_bPauseTimer = !GetComponent<EffectDispatcher>()->m_bPauseTimer;
			}
			else if (ulKey == VK_OEM_COMMA && ComponentExists<DebugMenu>() && GetComponent<DebugMenu>()->IsEnabled())
			{
				GetComponent<DebugMenu>()->SetVisible(!GetComponent<DebugMenu>()->IsVisible());
			}
			else if (ulKey == 0x4C && ms_bToggleModShortcutEnabled) // L
			{
				ms_bDisableMod = !ms_bDisableMod;
			}
		}

		if (ComponentExists<DebugMenu>())
		{
			GetComponent<DebugMenu>()->HandleInput(ulKey, bWasDownBefore);
		}

		if (ComponentExists<Shortcuts>())
		{
			GetComponent<Shortcuts>()->HandleInput(ulKey, bWasDownBefore);
		}
	}
}