#include <stdafx.h>

#include "Main.h"

#include "Effects/EffectConfig.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"
#include "Memory/Hooks/ShaderHook.h"
#include "Memory/Misc.h"
#include "Memory/Shader.h"

#include "Components/DebugMenu.h"
#include "Components/DebugSocket.h"
#include "Components/EffectDispatcher.h"
#include "Components/Failsafe.h"
#include "Components/KeyStates.h"
#include "Components/LuaScripts.h"
#include "Components/MetaModifiers.h"
#include "Components/Mp3Manager.h"
#include "Components/Shortcuts.h"
#include "Components/SplashTexts.h"
#include "Components/TwitchVoting.h"

#include "Util/File.h"
#include "Util/OptionsManager.h"
#include "Util/PoolSpawner.h"

static struct
{
	bool ClearAllEffects             = false;
	bool ClearEffectsShortcutEnabled = false;
	bool ToggleModShortcutEnabled    = false;
	bool DisableMod                  = false;
	bool PauseTimerShortcutEnabled   = false;
	bool HaveLateHooksRan            = false;
	bool AntiSoftlockShortcutEnabled = false;
	bool RunAntiSoftlock             = false;
} ms_Flags;

static std::array<BYTE, 3> ParseConfigColorString(const std::string &colorText)
{
	// Format: #ARGB
	std::array<BYTE, 3> colors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		Util::TryParse<BYTE>(colorText.substr(i, 2), colors[j++], 16);
	}

	return colors;
}

static void ParseEffectsFile()
{
	g_EnabledEffects.clear();

	EffectConfig::ReadConfig("chaosmod/effects.ini", g_EnabledEffects);
}

static void Reset()
{
	// Check if this isn't the first time this is being run
	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->Reset();
		while (GetComponent<EffectDispatcher>()->IsClearingEffects())
		{
			GetComponent<EffectDispatcher>()->OnRun();
			WAIT(0);
		}

		LOG("Mod has been disabled");
	}

	ClearEntityPool();

	for (auto component : g_Components)
	{
		component->OnModPauseCleanup();
	}
}

static void Init()
{
	// Attempt to print game build number
	// We're doing it here as the build number isn't available when the mod is attached to the game process
	static auto printedGameBuild = []()
	{
		auto gameBuild = Memory::GetGameBuild();
		if (gameBuild.empty())
		{
			gameBuild = "Unknown";
		}

		LOG("Game Build: " << gameBuild);

		return true;
	}();

	static std::streambuf *oldStreamBuf;
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

			oldStreamBuf = std::cout.rdbuf();

			g_ConsoleOut = std::ofstream("CONOUT$");
			std::cout.rdbuf(g_ConsoleOut.rdbuf());

			std::cout.clear();

			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			DWORD conMode;
			GetConsoleMode(handle, &conMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), conMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
	}
	else if (GetConsoleWindow())
	{
		LOG("Destroying log console");

		std::cout.rdbuf(oldStreamBuf);

		g_ConsoleOut.close();

		FreeConsole();
	}

	LOG("Parsing config files");
	ParseEffectsFile();

	g_OptionsManager.Reset();

	ms_Flags.ClearEffectsShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnableClearEffectsShortcut", OPTION_DEFAULT_SHORTCUT_CLEAR_EFFECTS);
	ms_Flags.ToggleModShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnableToggleModShortcut", OPTION_DEFAULT_SHORTCUT_TOGGLE_MOD);
	ms_Flags.PauseTimerShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnablePauseTimerShortcut", OPTION_DEFAULT_SHORTCUT_PAUSE_TIMER);
	ms_Flags.AntiSoftlockShortcutEnabled =
	    g_OptionsManager.GetConfigValue<bool>("EnableAntiSoftlockShortcut", OPTION_DEFAULT_SHORTCUT_ANTI_SOFTLOCK);

	g_EnableGroupWeighting =
	    g_OptionsManager.GetConfigValue<bool>("EnableGroupWeightingAdjustments", OPTION_DEFAULT_GROUP_WEIGHTING);

	const auto &timerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTimerColor", OPTION_DEFAULT_BAR_COLOR));
	const auto &textColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTextColor", OPTION_DEFAULT_TEXT_COLOR));
	const auto &effectTimerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>("EffectTimedTimerColor", OPTION_DEFAULT_TIMED_COLOR));

	g_Random.SetSeed(g_OptionsManager.GetConfigValue<int>("Seed", 0));

	LOG("Initializing effect sound system");
	InitComponent<Mp3Manager>();

	LOG("Initializing meta modifier states");
	InitComponent<MetaModifiers>();

	LOG("Initializing Lua scripts");
	InitComponent<LuaScripts>();

	LOG("Initializing effects dispatcher");
	InitComponent<EffectDispatcher>(timerColor, textColor, effectTimerColor);

	InitComponent<DebugMenu>();

	LOG("Initializing shortcuts handler");
	InitComponent<Shortcuts>();

	LOG("Initializing key state handler");
	InitComponent<KeyStates>();

	LOG("Initializing Twitch voting");
	InitComponent<TwitchVoting>(textColor);

	LOG("Initializing Failsafe");
	InitComponent<Failsafe>();

#ifdef WITH_DEBUG_PANEL_SUPPORT
	if (DoesFileExist("chaosmod\\.enabledebugsocket"))
	{
		LOG("Initializing Debug Websocket");
		InitComponent<DebugSocket>();
	}
#endif

	LOG("Completed init");

	if (ComponentExists<TwitchVoting>() && GetComponent<TwitchVoting>()->IsEnabled() && ComponentExists<SplashTexts>())
	{
		GetComponent<SplashTexts>()->ShowTwitchVotingSplash();
	}
}

static void MainRun()
{
	if (!ms_Flags.HaveLateHooksRan)
	{
		ms_Flags.HaveLateHooksRan = true;

		Memory::RunLateHooks();
	}

	g_MainThread = GetCurrentFiber();

	Reset();

	InitComponent<SplashTexts>();
	GetComponent<SplashTexts>()->ShowInitSplash();

	ms_Flags.DisableMod = g_OptionsManager.GetConfigValue<bool>("DisableStartup", OPTION_DEFAULT_DISABLE_STARTUP);

	Init();

	bool isDisabled = false;

	while (true)
	{
		WAIT(0);

		// This will run regardless if mod is disabled
		if (ms_Flags.RunAntiSoftlock)
		{
			ms_Flags.RunAntiSoftlock = false;
			if (IS_SCREEN_FADED_OUT())
			{
				DO_SCREEN_FADE_IN(0);
				SET_ENTITY_HEALTH(PLAYER_PED_ID(), 0, 0);
			}
		}

		if (ms_Flags.DisableMod && !isDisabled)
		{
			isDisabled = true;

			if (ComponentExists<EffectDispatcher>())
			{
				GetComponent<EffectDispatcher>()->Reset(EffectDispatcher::ClearEffectsFlag_NoRestartPermanentEffects);
				while (GetComponent<EffectDispatcher>()->IsClearingEffects())
				{
					GetComponent<EffectDispatcher>()->OnRun();
					WAIT(0);
				}
			}

			Reset();

			continue;
		}
		else if (isDisabled)
		{
			if (!ms_Flags.DisableMod)
			{
				isDisabled = false;

				LOG("Mod has been re-enabled");

				if (DoesFileExist("chaosmod\\.clearlogfileonreset"))
				{
					// Clear log
					g_Log = std::ofstream(CHAOS_LOG_FILE);
				}

				// Restart the main part of the mod completely
				Init();
			}

			continue;
		}

		if (ms_Flags.ClearAllEffects)
		{
			ms_Flags.ClearAllEffects = false;

			if (ComponentExists<EffectDispatcher>())
			{
				GetComponent<EffectDispatcher>()->Reset(EffectDispatcher::ClearEffectsFlag_NoRestartPermanentEffects);
				while (GetComponent<EffectDispatcher>()->IsClearingEffects())
				{
					GetComponent<EffectDispatcher>()->OnRun();
					WAIT(0);
				}
			}

			ClearEntityPool();
		}

		if (IS_SCREEN_FADED_OUT())
		{
			// Prevent potential softlock for certain effects
			SET_TIME_SCALE(1.f);
			Hooks::DisableScriptThreadBlock();
			WAIT(100);

			continue;
		}

		for (auto component : g_Components)
		{
			component->OnRun();
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
	}

	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore,
	                     BOOL isUpNow)
	{
		static bool isCtrlPressed  = false;
		static bool isShiftPressed = false;

		if (key == VK_CONTROL)
		{
			isCtrlPressed = !isUpNow;
		}
		else if (key == VK_SHIFT)
		{
			isShiftPressed = !isUpNow;
		}
		else if (isCtrlPressed && !wasDownBefore)
		{
			if (key == VK_OEM_MINUS)
			{
				if (ms_Flags.ClearEffectsShortcutEnabled)
				{
					ms_Flags.ClearAllEffects = true;

					if (ComponentExists<SplashTexts>())
					{
						GetComponent<SplashTexts>()->ShowClearEffectsSplash();
					}
				}
			}
			else if (key == VK_OEM_PERIOD)
			{
				if (ms_Flags.PauseTimerShortcutEnabled && ComponentExists<EffectDispatcher>())
				{
					GetComponent<EffectDispatcher>()->m_PauseTimer = !GetComponent<EffectDispatcher>()->m_PauseTimer;
				}
			}
			else if (key == VK_OEM_COMMA)
			{
				if (ComponentExists<DebugMenu>() && GetComponent<DebugMenu>()->IsEnabled())
				{
					GetComponent<DebugMenu>()->SetVisible(!GetComponent<DebugMenu>()->IsVisible());
				}
			}
			else if (key == 0x4B) // K
			{
				if (ms_Flags.AntiSoftlockShortcutEnabled && isShiftPressed)
				{
					ms_Flags.RunAntiSoftlock = true;
				}
			}
			else if (key == 0x4C) // L
			{
				if (ms_Flags.ToggleModShortcutEnabled)
				{
					ms_Flags.DisableMod = !ms_Flags.DisableMod;
				}
			}
		}

		for (auto component : g_Components)
		{
			component->OnKeyInput(key, wasDownBefore, isUpNow);
		}
	}
}
